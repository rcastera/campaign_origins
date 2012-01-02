<?php
// $Id$

/**
 * @file
 * Administer.
 *
 * Administering the campaign origins module.
 */

/**
 * Settings form to modify expiration date of cookie.
 */ 
function campaign_origins_admin_settings() {
  drupal_set_title(t('Campaign Origins Expiration'));
  
  $form['campaign_origins_expiration'] = array(
    '#type' => 'textfield',
    '#title' => t('Days'),
    '#default_value' => variable_get('campaign_origins_expiration', 7),
    '#description' => t('Days the cookie will expire. If set to 0, the cookie will expire at the end of the session (when the browser closes).'),
    '#required' => TRUE
  );

  return system_settings_form($form);
}

/**
 * Implementation of hook_validate().
 */ 
function campaign_origins_admin_settings_validate($form, &$form_state) {
  $form_state['values']['campaign_origins_expiration'] = intval($form_state['values']['campaign_origins_expiration']);
  if ((!is_numeric($form_state['values']['campaign_origins_expiration']))) {
    form_set_error('campaign_origins_expiration', t('Please enter a valid numerical value representing the number of days for the cookie.'));
  }
}

/**
 * Form to document/add new origin code to be tracked. 
 */ 
function campaign_origins_admin_add_origin() {
  drupal_set_title(t('Add Campaign Origin'));
  
  $form['campaign_origins_origin'] = array(
    '#type' => 'textfield',
    '#title' => t('Origin Code'),
    '#default_value' => '',
    '#size' => 60,
    '#maxlength' => 255,
    '#description' => t('Enter your origin code. (255) characters max. The following characters comma (,) or pipe (|) are not allowed.'),
    '#required' => TRUE
  );
  
  $form['campaign_origins_description'] = array(
    '#type' => 'textfield',
    '#title' => t('Description'),
    '#default_value' => '',
    '#size' => 60,
    '#maxlength' => 255,
    '#description' => t('Enter a description for this origin code. (255) characters max.'),
    '#required' => TRUE
  );
  
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Add')
  );

  return $form;
}

/**
 * Implementation of hook_validate().
 */ 
function campaign_origins_admin_add_origin_validate($form, &$form_state) {
  if ($form_state['values']['campaign_origins_origin'] == '') {
    form_set_error('campaign_origins_origin', t('Please enter an origin code.'));
  }
}

/**
 * Implementation of hook_submit().
 */ 
function campaign_origins_admin_add_origin_submit($form, &$form_state) {
  if (strpos($form_state['values']['campaign_origins_origin'], ',') || strpos($form_state['values']['campaign_origins_origin'], '|')) {
    form_set_error('campaign_origins_origin', t('The origin code cannot contain the following characters comma (,) or pipe (|).'));
    return;
  }
  
  if (db_result(db_query("SELECT COUNT(*) FROM {campaign_origins} WHERE origin = '%s'", $form_state['values']['campaign_origins_origin'])) > 0) {
    form_set_error('campaign_origins_origin', t('The origin code (' .$form_state['values']['campaign_origins_origin']. ') already exists.'));
    return;
  }

  db_query("INSERT INTO {campaign_origins} (origin, origin_description, created) VALUES ('%s', '%s', %d)", $form_state['values']['campaign_origins_origin'], $form_state['values']['campaign_origins_description'], time());
  drupal_set_message('Origin code added.', 'status');   
}

/**
 * View stored campaign origins.
 */
function campaign_origins_view_origins() {
  drupal_set_title(t('View Campaign Origins'));
  
  $number_results = 10;
  $columns = array(
    'id' => t('Id'),
    'origin' => t('Origin'),
    'origin_description' => t('Description'),
    'created' => t('Created'),
  );

  $sql = "SELECT * FROM {campaign_origins}";
  $sql_count = "SELECT COUNT(*) FROM {campaign_origins}";

  $header = array();
  foreach ($columns as $key => $value) {
    $header[] = array(
      'data' => $value, 'sort' => 'desc',
      'field' => $key,
    );
  }
  
  $sql .= tablesort_sql($header);
  $resource = pager_query($sql, $number_results, 0, $sql_count, implode(', ', array_keys($columns)));

  $results = array();
  while ($row = db_fetch_array($resource)) {
    $query['origin'] = $row['origin'];
    $row['origin'] = l($row['origin'], 'admin/reports/campaign-origins', array('query' => $query));
    $row['created'] = date('m-d-Y g:i A', $row['created']);
    $results[] = $row; 
  }

  $output = '';
  $output .= theme('table', $header, $results);
  $output .= theme('pager', NULL, $number_results, 0);
  return $output;
}

/**
 * Implementation of hook_theme().
 */
function campaign_origins_theme() {
  return array (
    'campaign_origins_table' => array(
      'arguments' => array('table' => array()),
    ),
    'campaign_origins_csv' => array(
      'arguments' => array('table' => array()),
    ),
  );
}

/**
 * Theme Table
 */
function theme_campaign_origins_table($table) {
  $out = '';
  $out .= '<table class="report">';
  
  if ($table['#columns']) {
    $out .= '<thead><tr>';
    foreach ($table['#columns'] as $field) {
      $out .= '<th>'.$field.'</th>';
    }
    $out .= '</tr></thead>';
  }
  
  $out .= '<tbody>';
  $zebra = 'odd';
  
  foreach ($table['#rows'] as $row) {
    $out .= '<tr class="'.$zebra.'">';
    foreach ($table['#columns'] as $field) {
      $out .= '<td>'.$row[$field].'</td>';
    }
    $out .= '</tr>';
    $zebra = ($zebra=='odd')?'even':'odd';
  }
  
  $out .= '</tbody>';
  $out .= '</table>';
  return $out;
}

if (!function_exists('str_putcsv')) {
  function str_putcsv($input, $delimiter = ',', $enclosure = '"') {
    $fp = fopen('php://temp', 'r+');
    fputcsv($fp, $input, $delimiter, $enclosure);
    rewind($fp);
    $data = fgets($fp);
    fclose($fp);
    return rtrim($data, "\n");
  }
}

/**
 * Theme CSV
 */
function theme_campaign_origins_csv($head, $rows) {
  $header_row = array();
  foreach ($head as $field) {
    $header_row[] = $field['data'];
  }
  
  $out = str_putcsv($header_row)."\n";
  $records = array();
  
  foreach ($rows as $row) {
    $out .= str_putcsv($row)."\n";
  }
  return $out;
}

/**
 * Time range form.
 */
function campaign_origins_time_range_form() {
  $form = '<form id="reports-form-time-range" name="reports-form-time-range" method="get" accept-charset="UTF-8" action="" style="margin: 10px 0px;">';
  $form .= '<div>';
  
  $form .= '<div id="edit-origin-wrapper" class="form-item">';
  $form .= '<label for="edit-origin">Origin Code: </label>';
  $form .= '<input id="edit-origin" class="form-text" type="text" value="' . $_REQUEST['origin'] . '" size="60" name="origin" maxlength="128"/>';
  $form .= '<div class="description">Enter an Origin code to search for</div>';
  $form .= '</div>';
  
  $form .= '<div id="edit-start-wrapper" class="form-item">';
  $form .= '<label for="edit-start">Start: </label>';
  $form .= '<input id="edit-start" class="form-text" type="text" value="' . $_REQUEST['start'] . '" size="60" name="start" maxlength="128"/>';
  $form .= '<div class="description">Enter a date in (YYYY-MM-DD) format</div>';
  $form .= '</div>';
  
  $form .= '<div id="edit-end-wrapper" class="form-item">';
  $form .= '<label for="edit-end">End: </label>';
  $form .= '<input id="edit-end" class="form-text" type="text" value="' . $_REQUEST['end'] . '" size="60" name="end" maxlength="128"/>';
  $form .= '<div class="description">Enter a date in (YYYY-MM-DD) format</div>';
  $form .= '</div>';
  
  $form .= '<input id="edit-submit" class="form-submit" type="submit" value="Filter" />';
  $form .= '<input id="edit-rest" class="form-submit" type="submit" value="Reset" onclick="location.href=\'/admin/reports/campaign-origins\'; return false;" />';
  
  
  if ($_REQUEST['sort']) {
    $form .= '<input type="hidden" name="sort" value="'.$_REQUEST['sort'].'">';
  }
  if ($_REQUEST['order']) {
    $form .= '<input type="hidden" name="order" value="'.$_REQUEST['order'].'">';
  }
  
  $form .= '</div>';
  $form .= '</form>';
  return $form;
}

/**
 * Displays the campaign origin report.
 * Modifies the query if an origin code is present.
 *
 * @param string $origin
 *   A string containing an origin code.
 */
function campaign_origins_show_origin_report() {
  $args = func_get_args();
  foreach ($args as $arg) {
    if (is_numeric($arg)) {
      if (!isset($page)) {$page = $arg;}
    } 
    else {
      if (!isset($format)) {$format = $arg;}
    }
  }
  
  if (!isset($page)) {$page = 1;}
  if (!isset($format)) {$format = 'table';}
  
  $head = array(
    array('data' => t('Origin Code'), 'field' => 'origin'),
    array('data' => t('Origination Page'), 'field' => 'node_id'),
    array('data' => t('Form'), 'field' => 'form_name'),
    array('data' => t('Record ID'), 'field' => 'form_record_id'),
    array('data' => t('Accessed'), 'field' => 'created', 'sort' => 'desc'),
  );
  
  $rows = array();
  $origin = $start = $end = $sql = $count = '';
  $params = array();
  
  if ($_REQUEST['origin']) {
    $origin = " WHERE o.origin = '%s'";
    $params[] = $_REQUEST['origin'];
  }
  
  if ($_REQUEST['start']) {
    if ($_REQUEST['origin']) {
      $start = " AND t.created >= %d";
      $params[] = strtotime($_REQUEST['start']); 
    }
    else {
      $start = " WHERE t.created >= %d";
      $params[] = strtotime($_REQUEST['start']);
    }
  }
  
  if ($_REQUEST['end']) {
    $end = " AND t.created <= %d";
    $params[] = strtotime($_REQUEST['end']." +1 day");
  }
  
  if ($_REQUEST['origin']) {
    $sql = "SELECT o.origin, t.node_id, t.form_name, t.form_record_id, t.created FROM {campaign_origins} o LEFT JOIN {campaign_origins_tracking} t ON t.campaign_origins_id = o.id".$origin.$start.$end;
    $count = "SELECT COUNT(*) FROM {campaign_origins} o LEFT JOIN {campaign_origins_tracking} t ON t.campaign_origins_id = o.id".$origin.$start.$end;  
  }
  else {
    $sql = "SELECT o.origin, t.node_id, t.form_name, t.form_record_id, t.created FROM {campaign_origins} o LEFT JOIN {campaign_origins_tracking} t ON t.campaign_origins_id = o.id".$start.$end;
    $count = "SELECT COUNT(*) FROM {campaign_origins} o LEFT JOIN {campaign_origins_tracking} t ON t.campaign_origins_id = o.id".$start.$end;
  }
  
  if ($format == 'csv') {
    $result = db_query($sql, $params);
  } 
  else {
    $limit = 20;
    $tablesort = tablesort_sql($head);
    $result = pager_query($sql.$tablesort, $limit, 0, $count, $params);
  }
  
  while ($record = db_fetch_array($result)) {
    if ($record['node_id']) {
      if ($format == 'csv') {
        $record['node_id'] = 'http://' . $_SERVER['SERVER_NAME'] . '/' . drupal_lookup_path('alias', 'node/' . $record['node_id']);
      }
      else {
        $record['node_id'] = l('http://' . $_SERVER['SERVER_NAME'] . '/' . drupal_lookup_path('alias', 'node/' . $record['node_id']), drupal_lookup_path('alias', 'node/' . $record['node_id'])); 
      }  
    }
    else {
      $record['node_id'] = 'No data';
    }
    $record['created'] = ($record['created'] == NULL) ? 'No data':date('m-d-Y g:i A', $record['created']);
    $rows[] = $record;
  }
  
  switch ($format) {
    case 'csv':
      drupal_set_header("Cache-Control: no-store, no-cache, must-revalidate");
      drupal_set_header("Content-type: application/csv");
      drupal_set_header("Content-Disposition: attachment; filename=\"origins.csv\"");
      print theme('reports_csv', $head, $rows);
      exit;
      
    default:
      drupal_set_title(t('Campaign Origins Report'));
      $query = array();
      if ($_REQUEST['start']) {$query['start'] = $_REQUEST['start'];}
      if ($_REQUEST['end']) {$query['end'] = $_REQUEST['end'];}
      if ($_REQUEST['origin']) {$query['origin'] = $_REQUEST['origin'];}
      
      global $pager_total_items;
      $summary = '<h3>Displaying '.$pager_total_items[0].' records</h3>';
      if ($pager_total_items[0] > 0) {
        $page = $_REQUEST['page'] > 0?(int)$_REQUEST['page']+1:1;
        $top = $page*$limit;
        $bottom = $top-$limit+1;
        $summary = '<h3>Displaying '.$bottom.'-'.$top.' of '.$pager_total_items[0].' records</h3>';
      }
      return $summary.'<p>'.l(t('Download as CSV'), 'admin/reports/campaign-origins/csv', array('query' => $query)).'</p>'.campaign_origins_time_range_form().theme('table', $head, $rows).theme('pager', NULL, $limit, 0, NULL, 5);
      break;
  }
  return '';
}