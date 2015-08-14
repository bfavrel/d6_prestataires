<?php

function prestataires_dispos_views_data() {
    
    $data['prestataires_dispos_dispos']['table']['group'] = t('Prestataires dispos table');
    
    $data['prestataires_dispos_dispos']['table']['join'] = array(
            'node' => array(
            'left_field' => 'nid', 
            'field' => 'nid',
        ),
    );
    
    // nid field
    $data['prestataires_dispos_dispos']['nid'] = array(
        'title' => t('nid'), 
        'help' => t('node id'),    
        'relationship' => array(
            'base' => 'node', 
            'field' => 'nid', 
            'handler' => 'views_handler_relationship', 
            'label' => t('nid join'),
        ),
    );
    
    // start_date field
    $data['prestataires_dispos_dispos']['start_date'] = array(
        'title' => t('Start date'),
        'help' => t('Dispo range start date'),
        'field' => array(
            'handler' => 'views_handler_field_date',
        ),
        'filter' => array(
            'handler' => 'prestataires_dispos_handler_filter_date',
        ),
        'sort' => array(
            'handler' => 'views_handler_sort_date',
        ),
    );
    
    // end_date field
    $data['prestataires_dispos_dispos']['end_date'] = array(
        'title' => t('End date'),
        'help' => t('Dispo range end date'),
        'field' => array(
            'handler' => 'views_handler_field_date',
        ),
        'filter' => array(
            'handler' => 'prestataires_dispos_handler_filter_date',
        ),
        'sort' => array(
            'handler' => 'views_handler_sort_date',
        ),
    );
    
    // type_dispo field
    $data['prestataires_dispos_dispos']['type_dispo'] = array(
        'title' => t('Dispo type'), 
        'help' => t('Dispo type'), 
        'field' => array(
            'handler' => 'views_handler_field_numeric', 
            'click sortable' => TRUE,
        ), 
        'filter' => array(
            'handler' => 'views_handler_filter_numeric',
        ), 
        'sort' => array(
            'handler' => 'views_handler_sort',
        ),
        'argument' => array(
            'handler' => 'views_handler_argument_numeric',
        ),
    );
    
    // data field
    $data['prestataires_dispos_dispos']['data'] = array(
        'title' => t('Data'), 
        'help' => t('Dispos data'), 
        'field' => array(
            'handler' => 'views_handler_field', 
            'click sortable' => TRUE,
        ), 
        'sort' => array(
            'handler' => 'views_handler_sort',
        ), 
        'filter' => array(
            'handler' => 'views_handler_filter_string',
        ), 
        'argument' => array(
            'handler' => 'views_handler_argument_string',
        ),
  );
    
    return $data;
}

/**
 *  Implementation of hook_views_handlers.
 */
function prestataires_dispos_views_handlers() {
    
    return array(
        'info' => array(
            'path' => drupal_get_path('module', 'prestataires_dispos') . "/views",
        ),
        'handlers' => array(
            'prestataires_dispos_handler_filter_date' => array(
                'parent' => 'views_handler_filter_date',
            ),
        ),
    );
}

/**
* Substitute current date; this works with cached queries.
*/
function prestataires_dispos_views_query_substitutions($view) {  
    
    $tdata = gmdate('Y-m-d', time());
    
    return array('***CURRENT_DATE***' => $tdata);
}