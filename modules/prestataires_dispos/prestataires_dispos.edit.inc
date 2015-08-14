<?php

function prestataires_dispos_edit_dispos_form(&$form_state, $nid) {

    $tnid = db_result(db_query("SELECT tnid FROM {node} WHERE nid=%d", $nid));

    if(!empty($tnid)) {$nid = $tnid;}

    $form = array();

    $form['nid'] = array(
        '#type' => 'hidden',
        '#value' => $nid
    );

    $form['calendar_editor'] = array(
        '#type' => 'markup',
        '#prefix' => "<div id='edit_dispo_controls'>",
        '#suffix' => "</div>",
    );

    $form['calendar_editor']['start_date_display'] = array(
        '#type' => 'markup',
        '#prefix' => "<div id='start_date_display'>",
        '#suffix' => "</div>",
        '#value' => t("Pick a start date"),
    );

    $form['calendar_editor']['start_date_value'] = array(
        '#type' => 'hidden',
    );

    $form['calendar_editor']['end_date_display'] = array(
        '#type' => 'markup',
        '#prefix' => "<div id='end_date_display' class='step_two' style='display: none;'>",
        '#suffix' => "</div>",
        '#value' => t("Optionaly, pick an end date."),
    );

    $form['calendar_editor']['end_date_value'] = array(
        '#type' => 'hidden',
    );

    $form['calendar_editor']['dispo_data'] = array(
        '#type' => 'fieldset',
        '#title' => t("Informations"),
        '#description' => t("Enter a number or a little text.<br />Fields filled with nothing or '0' will not appear."),
        '#attributes' => array('class' => 'step_two', 'style' => 'display: none;'),
    );

    $dispos_datafields = prestataires_dispos_get_dispos_datafields();

    foreach($dispos_datafields as $idx => $field) {
        $form['calendar_editor']['dispo_data']['dispo_field_' . $idx] = array(
            '#type' => 'textfield',
            '#title' => t($field),
            '#default_value' => null,
        );
    }

    $form['calendar_editor']['dispo_type_message'] = array(
        '#type' => 'markup',
        '#prefix' => "<div id='dispo_type_message_2' class='step_three' style='display: none;'>",
        '#suffix' => "</div>",
        '#value' => t("Select an availability type below."),
    );

    $form['calendar_editor']['dispo_type_0'] = array(
        '#type' => 'submit',
        '#value' => t("Not specified"),
        '#attributes' => array('class' => 'valid_dispo', 'style' => 'display: none;'),
        '#ahah' => array(
            'event' => 'click',
            'path' => 'prestataires/dispos/save',
            'wrapper' => 'edit_dispos_form',
            'effect' => 'fade',
            'method' => 'replace',
        ),
    );

    $dispos_types = prestataires_dispos_get_dispos_types();

    foreach($dispos_types as $idx => $dispo_type) {
        $form['calendar_editor']['dispo_type_' . ($idx +1)] = array(
            '#type' => 'submit',
            '#value' => t($dispo_type),
            '#attributes' => array('class' => 'valid_dispo', 'style' => 'display: none;'),
            '#ahah' => array(
                'event' => 'click',
                'path' => 'prestataires/dispos/save',
                'wrapper' => 'edit_dispos_form',
                'effect' => 'fade',
                'method' => 'replace',
            ),
        );
    }

    $form['calendar_editor']['cancel'] = array(
        '#type' => 'submit',
        '#value' => t('Cancel'),
        '#attributes' => array('class' => 'step_two', 'style' => 'display: none;'),
        '#ahah' => array(
            'event' => 'click',
            'path' => 'prestataires/dispos/cancel',
            'wrapper' => 'edit_dispo_controls',
            'effect' => 'fade',
            'method' => 'replace',
        ),
    );

    $form['calendar'] = array(
        '#type' => 'markup',
        '#prefix' => "<div id='edit_calendar' class='clearfix'>",
        '#suffix' => "</div>",
        '#value' => prestataires_dispos_render_calendar($nid, false),
    );

    $form['#submit'][] = 'prestataires_dispos_edit_dispos_form_submit';

    return $form;
}

/**
 * Form submission
 */
function prestataires_dispos_edit_dispos_form_submit($form, &$form_state) {

    // Force recently modified form to be displayed
    $form_state['rebuild'] = true;
}

/**
 * AHA callback for calendar submissions
 */
function prestataires_dispos_ajax_save() {

    $start_date = new DateTime($_POST['start_date_value']);
    $end_date = $_POST['end_date_value'] != null ? new DateTime($_POST['end_date_value']) : new DateTime($_POST['start_date_value']);
    $nid = $_POST['nid'];

    $op_dispo = $_POST['op'];
    $dispos_types_en = prestataires_dispos_get_dispos_types();
    $dispos_types = array();

    foreach($dispos_types_en as $key => $type) {
        $dispos_types[$key] = t($type);
    }

    $dispo_type = array_search($op_dispo, $dispos_types);

    if($dispo_type !== false) {
        $dispo_type += 1;
    }

    $dispo_data = array();

    $dispos_datafields = prestataires_dispos_get_dispos_datafields();

    foreach($dispos_datafields as $idx => $field) {
        $dispo_data[$idx] = check_plain($_POST['dispo_field_' . $idx]);
    }

    $dispo_data = serialize($dispo_data);

    if($start_date > $end_date) {
        list($start_date, $end_date) = array($end_date, $start_date);
    }

    // On recherche des périodes susceptibles de contenir au moins une des deux dates à enregistrer et on récupère les paramètres d'inclusion éventuelle
    $req = "SELECT *,
            (start_date <= '%s' AND end_date >= '%s') as start_included,
            (start_date <= '%s' AND end_date >= '%s') as end_included,
            (start_date > '%s' AND end_date < '%s') as surrounded
        FROM {prestataires_dispos_dispos} WHERE (
            (start_date <= '%s' AND end_date >= '%s') OR
            (start_date <= '%s' AND end_date >= '%s') OR
            (start_date > '%s' AND end_date < '%s'))
            AND nid=%d";

    $result = db_query($req,
            $start_date->format('Y-m-d'),
            $start_date->format('Y-m-d'),
            $end_date->format('Y-m-d'),
            $end_date->format('Y-m-d'),
            $start_date->format('Y-m-d'),
            $end_date->format('Y-m-d'),
            $start_date->format('Y-m-d'),
            $start_date->format('Y-m-d'),
            $end_date->format('Y-m-d'),
            $end_date->format('Y-m-d'),
            $start_date->format('Y-m-d'),
            $end_date->format('Y-m-d'),
            $nid);

    // on crée un enregistrement provisoire correspondant à la nouvelle période
    $range_to_insert = array(
        'nid' => $nid,
        'start_date' => $start_date->format('Y-m-d'),
        'end_date' => $end_date->format('Y-m-d'),
        'type_dispo' => $dispo_type,
        'dispo_data' => $dispo_data,
    );

    while($row = db_fetch_array($result)) {

        $old_start_date = new DateTime($row['start_date']);
        $old_end_date = new DateTime($row['end_date']);

        // S'il y a inclusion ou encadrement, il faut créer une nouvelle période. On supprime donc l'ancienne.
        if($row['start_included'] == 1 || $row['end_included'] == 1 || $row['surrounded'] == 1) {
            $req = "DELETE FROM {prestataires_dispos_dispos} WHERE id=%d";
            db_query($req, $row['id']);
        }

        // L'ancienne période inclue t-elle la date de départ de la nouvelle ?
        if($row['start_included'] == 1) {

            // Est-ce que la nouvelle période débute au début de l'ancienne ? Si oui, pas besoin de recréer l'ancienne période modifiée.
            if($start_date->modify('-1 day') >= $old_start_date) {

                // Est-ce que notre nouvelle période ne peut pas être fusionnée avec l'ancienne ? (même type_dispo et même data)
                if($row['type_dispo'] != $dispo_type || $row['data'] != $dispo_data) {

                    // Non. On en crée une.
                    $req = "INSERT INTO {prestataires_dispos_dispos} SET nid=%d, start_date='%s', end_date='%s', type_dispo=%d, data='%s'";
                    db_query($req, $row['nid'], $old_start_date->format('Y-m-d'), $start_date->format('Y-m-d'), $row['type_dispo'], $row['data']);

                } else {

                    // Oui. On étend la nouvelle période à l'ancienne.
                    $range_to_insert['start_date'] = $old_start_date->format('Y-m-d');
                }
            }
        }

        // L'ancienne période inclue t-elle la date de fin de la nouvelle ?
        if($row['end_included'] == 1) {

            if($end_date->modify('+1 day') <= $old_end_date) {

                if($row['type_dispo'] != $dispo_type || $row['data'] != $dispo_data) {

                    $req = "INSERT INTO {prestataires_dispos_dispos} SET nid=%d, start_date='%s', end_date='%s', type_dispo=%d, data='%s'";
                    db_query($req, $row['nid'], $end_date->format('Y-m-d'), $old_end_date->format('Y-m-d'), $row['type_dispo'], $row['data']);

                } else {
                    $range_to_insert['end_date'] = $old_end_date->format('Y-m-d');
                }
            }
        }
    }

    if($dispo_type != null) {
        $req = "INSERT INTO {prestataires_dispos_dispos} SET nid=%d, start_date='%s', end_date='%s', type_dispo=%d, data='%s'";
        db_query($req, $nid, $range_to_insert['start_date'], $range_to_insert['end_date'], $range_to_insert['type_dispo'], $range_to_insert['dispo_data']);
    }

    $req = "UPDATE {prestataires_dispos_nodes} SET last_update=NOW() WHERE nid=$nid";
    db_query($req);

    return drupal_json(drupal_get_form('prestataires_dispos_edit_dispos_form', $nid));
}

function prestataires_dispos_save($nid, $op_dispo, $start_date, $end_date = null, $data_field = array()) {

	$start_date = new DateTime($start_date);
	$end_date = $end_date != null ? new DateTime($end_date) : $start_date;

	$dispos_types_en = prestataires_dispos_get_dispos_types();

	$dispos_types = array();

	foreach($dispos_types_en as $key => $type) {
		$dispos_types[$key] = t($type);
	}

	$dispo_type = array_search($op_dispo, $dispos_types);

    if($dispo_type === false) {
        $dispo_type = array_search($op_dispo, $dispos_types_en);
    }

	if($dispo_type !== false) {
		$dispo_type += 1;
	}

	$dispo_data = array();

	if (!empty($data_field)) {
		$dispos_datafields = prestataires_dispos_get_dispos_datafields();

		foreach($dispos_datafields as $idx => $field) {
			$dispo_data[$idx] = check_plain($data_field[$idx]);
		}
	}

	$dispo_data = serialize($dispo_data);

	if($start_date > $end_date) {
		list($start_date, $end_date) = array($end_date, $start_date);
	}

	// On recherche des périodes susceptibles de contenir au moins une des deux dates à enregistrer et on récupère les paramètres d'inclusion éventuelle
	$req = "SELECT *,
	(start_date <= '%s' AND end_date >= '%s') as start_included,
	(start_date <= '%s' AND end_date >= '%s') as end_included,
	(start_date > '%s' AND end_date < '%s') as surrounded
	FROM {prestataires_dispos_dispos} WHERE (
	(start_date <= '%s' AND end_date >= '%s') OR
	(start_date <= '%s' AND end_date >= '%s') OR
	(start_date > '%s' AND end_date < '%s'))
	AND nid=%d";

	$result = db_query($req,
			$start_date->format('Y-m-d'),
			$start_date->format('Y-m-d'),
			$end_date->format('Y-m-d'),
			$end_date->format('Y-m-d'),
			$start_date->format('Y-m-d'),
			$end_date->format('Y-m-d'),
			$start_date->format('Y-m-d'),
			$start_date->format('Y-m-d'),
			$end_date->format('Y-m-d'),
			$end_date->format('Y-m-d'),
			$start_date->format('Y-m-d'),
			$end_date->format('Y-m-d'),
			$nid);

	// on crée un enregistrement provisoire correspondant à la nouvelle période
	$range_to_insert = array(
			'nid' => $nid,
			'start_date' => $start_date->format('Y-m-d'),
			'end_date' => $end_date->format('Y-m-d'),
			'type_dispo' => $dispo_type,
			'dispo_data' => $dispo_data,
	);

	while($row = db_fetch_array($result)) {

		$old_start_date = new DateTime($row['start_date']);
		$old_end_date = new DateTime($row['end_date']);

		// S'il y a inclusion ou encadrement, il faut créer une nouvelle période. On supprime donc l'ancienne.
		if($row['start_included'] == 1 || $row['end_included'] == 1 || $row['surrounded'] == 1) {
			$req = "DELETE FROM {prestataires_dispos_dispos} WHERE id=%d";
			db_query($req, $row['id']);
		}

		// L'ancienne période inclue t-elle la date de départ de la nouvelle ?
		if($row['start_included'] == 1) {

			// Est-ce que la nouvelle période débute au début de l'ancienne ? Si oui, pas besoin de recréer l'ancienne période modifiée.
			if($start_date->modify('-1 day') >= $old_start_date) {

				// Est-ce que notre nouvelle période ne peut pas être fusionnée avec l'ancienne ? (même type_dispo et même data)
				if($row['type_dispo'] != $dispo_type || $row['data'] != $dispo_data) {

					// Non. On en crée une.
					$req = "INSERT INTO {prestataires_dispos_dispos} SET nid=%d, start_date='%s', end_date='%s', type_dispo=%d, data='%s'";
					db_query($req, $row['nid'], $old_start_date->format('Y-m-d'), $start_date->format('Y-m-d'), $row['type_dispo'], $row['data']);

				} else {

					// Oui. On étend la nouvelle période à l'ancienne.
					$range_to_insert['start_date'] = $old_start_date->format('Y-m-d');
				}
			}
		}

		// L'ancienne période inclue t-elle la date de fin de la nouvelle ?
		if($row['end_included'] == 1) {

			if($end_date->modify('+1 day') <= $old_end_date) {

				if($row['type_dispo'] != $dispo_type || $row['data'] != $dispo_data) {

					$req = "INSERT INTO {prestataires_dispos_dispos} SET nid=%d, start_date='%s', end_date='%s', type_dispo=%d, data='%s'";
					db_query($req, $row['nid'], $end_date->format('Y-m-d'), $old_end_date->format('Y-m-d'), $row['type_dispo'], $row['data']);

				} else {
					$range_to_insert['end_date'] = $old_end_date->format('Y-m-d');
				}
			}
		}
	}

	if($dispo_type != null) {
		$req = "INSERT INTO {prestataires_dispos_dispos} SET nid=%d, start_date='%s', end_date='%s', type_dispo=%d, data='%s'";
		db_query($req, $nid, $range_to_insert['start_date'], $range_to_insert['end_date'], $range_to_insert['type_dispo'], $range_to_insert['dispo_data']);
	}

	$req = "UPDATE {prestataires_dispos_nodes} SET last_update=NOW() WHERE nid='$nid'";
	db_query($req);

	return true;
}

function prestataires_dispos_cancel() {

    $output = "";
    $form_build_id = $_POST['form_build_id'];
    $form_id = $_POST['form_id'];

    $form_state = array('storage' => null, 'submitted' => false);

    $form = form_get_cache($form_build_id, $form_state);
    $form = form_builder($form_id, $form, $form_state);

    foreach (element_children($form['calendar_editor']) as $element) {
        $output .= drupal_render($form['calendar_editor'][$element]);
    }

    return drupal_json($output);
}