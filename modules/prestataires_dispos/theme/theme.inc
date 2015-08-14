<?php

function prestataires_dispos_preprocess_node(&$vars) {

    $nid = $vars['nid'];

    $tnid = db_result(db_query("SELECT tnid FROM {node} WHERE nid=%d", $nid));

    if(!empty($tnid)) {$nid = $tnid;}

    $vars['dispos_last_update'] = _prestataires_dispos_get_last_update($nid);
}

function prestataires_dispos_preprocess_prestataires_dispos_page(&$vars) {

    $nid = $vars['nid'];

    $tnid = db_result(db_query("SELECT tnid FROM {node} WHERE nid=%d", $nid));

    if(!empty($tnid)) {$nid = $tnid;}

    $vars['dispos_last_update'] = _prestataires_dispos_get_last_update($nid);

    if($vars['dispos_last_update'] == null) {return;}

    $vars['legend'] = theme('prestataires_dispos_legend');

    $vars['calendar'] = prestataires_dispos_render_calendar($nid);

    // #dispos_data_display id is used by prestataires_dispos.js
    $vars['infos'] = "<div id='dispos_data_display'>" . t("Roll over a date to see availabilities details.") . "</div>";
}

function _prestataires_dispos_get_last_update($nid) {

    $req = "SELECT DISTINCT(last_update) FROM {prestataires_dispos_nodes} AS pdn
            RIGHT JOIN {prestataires_dispos_dispos} AS pdd ON pdd.nid=pdn.nid
            WHERE pdn.nid=%d AND pdd.end_date>= CURRENT_DATE()";
    $result = db_result(db_query($req, $nid));

    if(empty($result)) {return;}

    $last_update = DateTime::createFromFormat('Y-m-d H:i:s', $result);

    return t("Last update") . " : " . $last_update->format('d/m/Y') . " " . t("at") . " " . $last_update->format('H\hi');
}

function theme_prestataires_dispos_edit_form($nid) {

    $output = "";

    $req = "SELECT title FROM {node} WHERE nid=%d";
    $title = db_result(db_query($req, $nid));
    drupal_set_title($title);

    if(user_access('edit all dispos')) {
        $uid = db_result(db_query("SELECT uid FROM {prestataires_dispos_nodes} WHERE nid=%d", array($nid)));

        $output .= l("Cliquez ici pour accéder au compte utilisateur", 'user/' . $uid . '/edit', array('attributes' => array('target' => '_blank')));
    }

    $output .= l("Cliquez ici pour voir la fiche", drupal_get_path_alias('node/' . $nid), array('attributes' => array('target' => '_blank', 'id' => 'dispos_fiche_link')));

    $output .= "<div id='edit_dispos_data_display'><h3>" . t("Availabilities") . "</h3><div id='dispos_data_display'>" . t("Roll over a date to see availabilities details.") . "</div></div>";

    $output .= "<div id='edit_dispos_form'>";
        $output .= drupal_get_form('prestataires_dispos_edit_dispos_form', $nid);
    $output .= "</div>";

    $output .= "<div id='edit_dispos_calendar_legend'>";
        $output .= "<h3>" . t("Legend") . "</h3>";
        $output .= theme('prestataires_dispos_legend');
    $output .= "</div>";

    return $output;
}

function theme_prestataires_dispos_legend() {

    $output = "";

    $dispos_types = prestataires_dispos_get_dispos_types();
    $output .= "<div class='calendar_legend_item'><div class='dispo_type_0'></div><span> " . t("Not specified") . "</span></div>";

    foreach($dispos_types as $key => $dispo_type) {
        $output .= "<div class='calendar_legend_item'><div class='dispo_type_" . ($key +1) . "'></div><span> " . t("$dispo_type") . "</span></div>";
    }

    return $output;
}

function prestataires_dispos_preprocess_prestataires_dispos_kiosque_page(&$vars) {

    $content_type = $vars['arguments'][0][0];

    $contents_to_display = array_filter(variable_get('prestataires_kiosque_content_types', array()));

    $dispos_content_types = variable_get('prestataires_dispos_content_types', array());
    $formated_dispos_content_types = array();

    foreach ($dispos_content_types as $value) {
        if($value != null) {
            $formated_dispos_content_types[] = $value;
        }
    }

    asort($formated_dispos_content_types);

    if($content_type == null) {
        reset($contents_to_display);
        $content_type = current($contents_to_display);

        $content_type = $content_type == null ? $formated_dispos_content_types[0] : $content_type;
    }

    $content_type_names = node_get_types('names');
    $vars['title'] = t("Availabilities") . " " . strtolower($content_type_names[$content_type]);

    $vars['navigation'] = "";

    $vars['navigation'] .= "<ul id='prestataires_dispos_kiosque_menu'>";

    foreach($formated_dispos_content_types as $value) {

        if(!in_array($value, $contents_to_display)){continue;}

        if($value == $content_type)
        {
            $selected = "selected";
        } else {
            $selected = "";
        }

        $vars['navigation'] .= "<li>" . l($content_type_names[$value], 'kiosque/dispos/' . $value, array('attributes' => array('class' => $selected))) . "<li>";
    }

    $vars['navigation'] .= "</ul>";

    $kiosque_view = views_get_view('kiosque_prestataires_dispos');
    $vars['dispos'] = $kiosque_view->preview('default', array($content_type));
}

/**
 * Build the XML file data
 *
 * @param array $nodes
 * @param string $content_type_name : human readable
 * @param string $from
 * @param string $to
 *
 * @return string
 */
function theme_prestataires_dispos_export($nodes, $cols_header, $cols_style, $content_type_name, $from, $to) {

    require_once "sites/all/libraries/PHPExcel/Classes/PHPExcel.php";

    $xls = new PHPExcel();

    $xls->getProperties()->setTitle(t("Availabilities") . " " . $content_type_name . " " . t("from date") . " " . $from . " " . t("to date") . " " . $to);
    $xls->getProperties()->setSubject(t("Availabilities") . " " . $content_type_name . " " . t("from date") . " " . $from . " " . t("to date") . " " . $to);
    $xls->getProperties()->setCreator(variable_get('site_name', ''));
    $xls->getProperties()->setLastModifiedBy("Webdev");

    $xls->setActiveSheetIndex(0);
    $sheet=$xls->getActiveSheet();
    $sheet->setTitle(t("Availabilities"));

    $data = array('cols_header' => $cols_header) + $nodes;
    $cols_style = array_values($cols_style);

    //----- styles
    $x = 'A';

    foreach($cols_style as $style) {
        $sheet->getColumnDimension($x)->setWidth($style['width']);

        $x++;
    }

    $sheet->getRowDimension('1')->setRowHeight(30);

    //----- cellules
    $y = 1;

    foreach($data as $row) {

        $x = 'A';
        $x2 = 0;

        foreach($row as $cell) {
            if(is_array($cell) && empty($cell['bg_color'])) {
                $cell['bg_color'] = 'ffffff';
            }

            $sheet->getStyle((string)($x . $y))->getBorders()->getAllBorders()->setBorderStyle('thin');

            if($y > 1) {
                $sheet->getStyle((string)($x . $y))->getAlignment()->setHorizontal($cols_style[$x2]['horizontal']);
                $sheet->getStyle((string)($x . $y))->getAlignment()->setVertical(!empty($cols_style[$x2]['vertical']) ? $cols_style[$x2]['vertical'] : 'top');
                $sheet->getStyle((string)($x . $y))->getAlignment()->setWrapText(true);
            } else {
                $sheet->getStyle((string)($x . $y))->getFont()->setBold(true);
                $sheet->getStyle((string)($x . $y))->getAlignment()->setHorizontal('center');
                $sheet->getStyle((string)($x . $y))->getAlignment()->setVertical('center');
            }

            if(!is_array($cell)) {
                $sheet->SetCellValue((string)($x . $y), $cell);
            } else {
                $sheet->getStyle((string)($x . $y))->getFill()->setFillType('solid');
                $sheet->getStyle((string)($x . $y))->getFill()->getStartColor()->setRGB($cell['bg_color']);

                $sheet->SetCellValue((string)($x . $y), $cell['value']);
            }

            $x++;
            $x2++;
        }

        $y++;
    }

    $filename = $content_type_name . '_' . str_replace('/', '-', $from) . '_' . str_replace('/', '-', $to) . '.xlsx';

    drupal_set_header('Content-Type: application/vnd.openxmlformats-officedocument.spreadsheetml.sheet');
    drupal_set_header('Content-Disposition: attachment;filename="' . $filename . '"');
    drupal_set_header('Cache-Control: max-age=0');

    $objWriter = new PHPExcel_Writer_Excel2007($xls);
    $objWriter->setOffice2003Compatibility(true);
    $objWriter->save('php://output');

    // TODO : Faut-il appeler les hook_exit()...?
    exit(); // sinon, la sortie Drupal sera également écrite dans le fichier xls
}

// 19/05/2015 : page des dispos
function theme_prestataires_dispos_page_table($nodes, $cols_header, $content_label) {

    $output = "";

    if(empty($nodes)) {
        return "<p>- Aucun établissement de type " . $content_label . " n'a de disponibilité pour le moment -</p>";
    }

    $rows = array(
        array(
            'data' => array(
                array(
                    'data' => "<h2>" . $content_label . "</h2>",
                    'colspan' => 4,
                ),
                array(
                    'data' => "<strong>Légende:</strong><br />" .
                              "<strong>D</strong> = Disponible / Available<br />" .
                              "<strong>C</strong> = Complet / Fully booked<br />" .
                              "<strong>P</strong> = Peu de disponibilités<br />",
                    'colspan' => 3,
                ),
            ),
            'class' => 'subheader',
        ),
    );

    $rows[] = array(
        'data' => $cols_header,
        'class' => 'subheader',
    );

    foreach($nodes as $nid => $infos) {

        $rows[$nid] = array();

        $idx_col = 1;

        foreach($infos as $info) {
            $rows[$nid][] = array(
                'data' => $info,
                'class' => 'col_' . $idx_col,
            );

            $idx_col++;
        }
    }

    $output .= theme('table', array(), $rows, array('class' => 'dispo_page_table'));

    return $output;
}