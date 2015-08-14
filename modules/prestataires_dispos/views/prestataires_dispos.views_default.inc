<?php

/*
* Implementation of hook_views_default_views()
*/
function prestataires_dispos_views_default_views() {
    
    $view = new view;
    $view->name = 'kiosque_prestataires_dispos';
    $view->description = t("Displays daily availabilities");
    $view->tag = '';
    $view->base_table = 'node';
    $view->core = 6;
    $view->api_version = '2';
    $view->disabled = FALSE; /* Edit this to true to make a default view disabled initially */
    $handler = $view->new_display('default', 'Paramètres par défaut', 'default');
    $handler->override_option('relationships', array(
    'nid' => array(
        'label' => 'nid join',
        'required' => 1,
        'id' => 'nid',
        'table' => 'prestataires_dispos_dispos',
        'field' => 'nid',
        'relationship' => 'none',
    ),
    ));
    $handler->override_option('fields', array(
    'title' => array(
        'label' => 'Titre',
        'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
        ),
        'empty' => '',
        'hide_empty' => 0,
        'empty_zero' => 0,
        'hide_alter_empty' => 1,
        'link_to_node' => 0,
        'exclude' => 0,
        'id' => 'title',
        'table' => 'node',
        'field' => 'title',
        'relationship' => 'none',
    ),
    'data' => array(
        'label' => 'data',
        'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
        ),
        'empty' => '',
        'hide_empty' => 0,
        'empty_zero' => 0,
        'hide_alter_empty' => 1,
        'exclude' => 0,
        'id' => 'data',
        'table' => 'prestataires_dispos_dispos',
        'field' => 'data',
        'relationship' => 'none',
    ),
    'type_dispo' => array(
        'label' => 'Dispo type',
        'alter' => array(
        'alter_text' => 0,
        'text' => '',
        'make_link' => 0,
        'path' => '',
        'absolute' => 0,
        'link_class' => '',
        'alt' => '',
        'rel' => '',
        'prefix' => '',
        'suffix' => '',
        'target' => '',
        'help' => '',
        'trim' => 0,
        'max_length' => '',
        'word_boundary' => 1,
        'ellipsis' => 1,
        'html' => 0,
        'strip_tags' => 0,
        ),
        'empty' => '',
        'hide_empty' => 0,
        'empty_zero' => 0,
        'hide_alter_empty' => 1,
        'set_precision' => FALSE,
        'precision' => 0,
        'decimal' => '.',
        'separator' => ',',
        'format_plural' => 0,
        'format_plural_singular' => '1',
        'format_plural_plural' => '@count',
        'prefix' => '',
        'suffix' => '',
        'exclude' => 0,
        'id' => 'type_dispo',
        'table' => 'prestataires_dispos_dispos',
        'field' => 'type_dispo',
        'relationship' => 'none',
    ),
    ));
    $handler->override_option('filters', array(
    'status' => array(
        'operator' => '=',
        'value' => '1',
        'group' => '0',
        'exposed' => FALSE,
        'expose' => array(
        'operator' => FALSE,
        'label' => '',
        ),
        'id' => 'status',
        'table' => 'node',
        'field' => 'status',
        'relationship' => 'none',
    ),
    'start_date' => array(
        'operator' => '<=',
        'value' => array(
        'type' => 'offset',
        'value' => 'now',
        'min' => '',
        'max' => '',
        ),
        'group' => '0',
        'exposed' => FALSE,
        'expose' => array(
        'operator' => FALSE,
        'label' => '',
        ),
        'id' => 'start_date',
        'table' => 'prestataires_dispos_dispos',
        'field' => 'start_date',
        'relationship' => 'none',
    ),
    'end_date' => array(
        'operator' => '>=',
        'value' => array(
        'type' => 'offset',
        'value' => 'now',
        'min' => '',
        'max' => '',
        ),
        'group' => '0',
        'exposed' => FALSE,
        'expose' => array(
        'operator' => FALSE,
        'label' => '',
        ),
        'id' => 'end_date',
        'table' => 'prestataires_dispos_dispos',
        'field' => 'end_date',
        'relationship' => 'none',
    ),
    ));
    $handler->override_option('access', array(
    'type' => 'none',
    ));
    $handler->override_option('cache', array(
    'type' => 'none',
    ));
    $handler->override_option('use_pager', '1');

    $views[$view->name] = $view;
    return $views;
}