<?php

function prestataires_dispos_export_form($form_state) {
    drupal_set_title(t("Availabilities export"));

    $form = array();

    $form['#redirect'] = false; // utile simplement pour le dév, pour voir les dpm()
    $form['#tree'] = true;

    $form['content_type'] = array(
        '#type' => 'select',
        '#title' => t("Content type"),
        '#options' => array_filter(variable_get('prestataires_dispos_content_types', array())),
    );
    
    $form['status'] = array(
        '#type' => 'checkbox',
        '#title' => t("Only published nodes"),
        '#default_value' => 1,
    );

    $form['dates']['from'] = array(
        '#type' => 'date_popup',
        '#title' => t("From date"),
        '#default_value' => date('Y-m-d'),
        '#date_format' => 'd/m/Y',
    );

    $form['dates']['to'] = array(
        '#type' => 'date_popup',
        '#title' => t("To date"),
        '#date_format' => 'd/m/Y',
    );

    $form['submit'] = array(
        '#type' => 'submit',
        '#value' => t("Export"),
    );

    return $form;
}

function prestataires_dispos_export_form_validate($form, &$form_state) {

    if(empty($form_state['values']['dates']['from'])) {
        form_set_error('from', t("You must provide a start date"));
    }

    if(empty($form_state['values']['dates']['to'])) {
        form_set_error('to', t("You must provide an end date"));
    }

    if(!empty($form_state['values']['dates']['from']) && !empty($form_state['values']['dates']['to'])) {
        $from = DateTime::createFromFormat('Y-m-d H:i:s', $form_state['values']['dates']['from']);
        $to = DateTime::createFromFormat('Y-m-d H:i:s', $form_state['values']['dates']['to']);

        $diff = $from->diff($to);

        if($diff->days > variable_get('prestataires_dispos_limit_export_period', 31)) {
            if(variable_get('prestataires_dispos_allocate_memory', null) == null) {
                form_set_error('dates', t("Interval between the two dates can't exceed @days days", array('@days' => variable_get('prestataires_dispos_limit_export_period', 31))));
            } else {
                $form_state['storage']['memory_allocation'] = variable_get('prestataires_dispos_allocate_memory', null);
            }
        }
    }
}

function prestataires_dispos_export_form_submit($form, &$form_state) {

    if(!empty($form_state['storage']['memory_allocation'])) {
        ini_set('memory_limit', $form_state['storage']['memory_allocation']);
    }

    $dispos_types = variable_get('prestataires_dispos_dispos_types', "Closed|c2c2c2" . PHP_EOL . "Full|ffdede" . PHP_EOL . "Limited|fbff92" . PHP_EOL . "Available|c3ffc2");
    $dispos_types = explode(PHP_EOL, $dispos_types);

    array_walk($dispos_types, function(&$val){
        $val = explode('|', $val);
    });

    $dispos_fields_tmp = variable_get('prestataires_dispos_export_fields', array());
    $dispos_fields_tmp = explode(PHP_EOL, $dispos_fields_tmp);

    $dispos_fields = array();
    $cols_style = array();

    foreach($dispos_fields_tmp as $field) {
        $field = explode('|', $field);
        $dispos_fields[$field[0]] = $field[1];

        $tmp = explode(';', $field[2]);

        $cols_style[$field[0]] = array(
            'width' => $tmp[0],
            'horizontal' => $tmp[1],
            'vertical' => $tmp[2],
        );

        $cols_style[$field[0]] = array_filter($cols_style[$field[0]]);
    }

    $from = DateTime::createFromFormat('Y-m-d H:i:s', $form_state['values']['dates']['from']);
    $to = DateTime::createFromFormat('Y-m-d H:i:s', $form_state['values']['dates']['to']);
    $interval = new DateInterval('P1D');

    $to->add($interval); // DatePeriod exclut la date de fin

    $period = new DatePeriod($from, $interval, $to);

    $dispos_default = array();

    // on génère un array "modèle", indexé par chaque date de la période
    foreach($period as $date) {
        $dispos_default[$date->format('d/m/Y')] = '-'; // ou : t("Not specified"); trop lourd visuellement dans le xls
    }

    $req = "SELECT DISTINCT dn.nid, n.title, dn.last_update FROM {prestataires_dispos_nodes} AS dn
            LEFT JOIN {node} AS n ON n.nid=dn.nid
            LEFT JOIN {prestataires_dispos_dispos} AS dd ON dd.nid=dn.nid";
            
    $joins = variable_get('prestataires_dispos_export_joins', '');
    
    if($joins != '') {
        $req .= " " . $joins;
    }
    
    $req .= " WHERE n.type='%s'";

    if($form_state['values']['status'] == 1) {
        $req .= " AND n.status=1";
    }

    if(variable_get('prestataires_dispos_export_mode', 1) == 1) {
        $req .= " AND (
                (dd.start_date>='%s' AND dd.start_date<='%s')
                OR (dd.end_date>='%s' AND dd.end_date<='%s')
                OR (dd.start_date<='%s' AND dd.end_date>='%s')
                )";
    }
    
    $req.= " ORDER BY " . variable_get('prestataires_dispos_export_orderby', 'n.title ASC');

    $result = db_query($req,
        $form_state['values']['content_type'],
        $form_state['values']['dates']['from'],
        $form_state['values']['dates']['to'],
        $form_state['values']['dates']['from'],
        $form_state['values']['dates']['to'],
        $form_state['values']['dates']['from'],
        $form_state['values']['dates']['to']
    );

    $nodes = array();

    while($row = db_fetch_object($result)) {
        $nodes[$row->nid] = array();

        $node = node_load($row->nid);

        if(module_exists('sirtaqui_content_listing')) {

            foreach($dispos_fields as $field_name => $label) {

                if(!isset($node->{$field_name})) { // ce champ n'appartient pas à ce type de contenu
                    unset($dispos_fields[$field_name]);
                    unset($cols_style[$field_name]);
                    continue;
                }

                $values = array();

                foreach((array)$node->{$field_name} as $node_field) {
                    $values[] = is_array($node_field) ? $node_field['value'] : $node_field;
                }

                $values = array_filter($values);

                if(empty($values)) {
                    foreach((array)$node->{normalize_field_name('field_f_' . str_replace('field_', '', $field_name))} as $node_field) {
                        $values[] = is_array($node_field) ? $node_field['value'] : $node_field;
                    }
                }

                $values = implode(' - ', $values);

                $nodes[$row->nid][$field_name] = $values;
            }
        }

        $sub_req = "SELECT start_date, end_date, type_dispo FROM {prestataires_dispos_dispos} WHERE nid=%d";
        $sub_result = db_query($sub_req, $row->nid);

        $dispos = $dispos_default;

        while($sub_row = db_fetch_object($sub_result)) {
            $period_from = DateTime::createFromFormat('Y-m-d', $sub_row->start_date);
            $period_to = DateTime::createFromFormat('Y-m-d', $sub_row->end_date);

            $period_to->add($interval); // DatePeriod exclut la date de fin

            $period = new DatePeriod($period_from, $interval, $period_to);

            foreach($period as $date) {
                $dispos[$date->format('d/m/Y')] = array('value' => t($dispos_types[$sub_row->type_dispo - 1][0]), 'bg_color' => $dispos_types[$sub_row->type_dispo - 1][1]);
            }
        }

        $dispos = array_intersect_key($dispos, $dispos_default);

        $last_update = DateTime::createFromFormat('Y-m-d H:i:s', $row->last_update);
        $nodes[$row->nid]['last_update'] = $last_update->format('d/m/Y') . ' ' . t("at") . ' ' . $last_update->format('H\hi');

        $nodes[$row->nid] += $dispos;
    }

    $dispos_fields[] = t("Last update");
    $cols_style[] = array('width' => '20', 'horizontal' => 'center', 'vertical' => 'center');

    $cols_header = array_merge(array_values($dispos_fields), array_keys($dispos_default));

    $cols_style = array_pad($cols_style, count($cols_style) + count($dispos_default), array('width' => '20', 'horizontal' => 'center', 'vertical' => 'center'));

    $content_type_name = node_get_types('name', $form_state['values']['content_type']);

    theme('prestataires_dispos_export', $nodes, $cols_header, $cols_style, $content_type_name, $from->format('d/m/Y'), $to->format('d/m/Y'));
}



