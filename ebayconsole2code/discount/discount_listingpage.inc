<?php
/**
 * Implements callback of discount/list_all/distributor_list
 */
function discount_list_all_distributor_list(){
  $header = array();
  $options = stock_get_all_distributors();
  $default_value = array();
  $IsCheckbox = true;
  $header = array('publisher_name' => 'Publisher Name','category_id' => 'Category','discount' => 'Promo Discount',);
  if($IsCheckbox){
    $default_value[0] = true;
    $default_value[1] = false;
  }else{
    $default_value = '0';
  }
  
  $form['#method'] = 'GET';
  $form['distributore_selector'] = array (
    '#type' => 'fieldset',
    '#title' => 'Select Distributer',
    '#header' => t('Search'),    
  );
  
  $form['distributore_selector']['distributor_name']  =  array (
    '#type' => 'select',
    '#title' => 'Operation',
    '#options' => $options,       
  ); 
   
  $form ['distributore_selector']['distributor_search_submit'] = array(
    '#type' => 'submit', 
    '#weight' => 9,
    '#value' => t('Search'),
    '#prefix' => '<div class="stock_search_submit">',
    '#suffix' => '</div>',
  );
  
  $form['markup'] = array(
    '#type' => 'markup',
    '#markup' => discount_list_all_distributor_list_table(),    
  );
  return $form;
}

function discount_list_all_distributor_list_table(){
  ctools_include('modal');
  ctools_include('ajax');
  ctools_modal_add_js();
  
  $distributor_id= $_GET['distributor_name'];
  
  $header = array('Publisher Name','Category','Promo Discount','operation');
  $get_distributor_list= get_list_by_distributor_id($distributor_id);
  $rows = array();
  
  foreach($get_distributor_list as $rec){
    $row = array();
    $row[]=$rec->publisher_name;   
    $row[]=$rec->category_id; 
    $row[]=$rec->discount;   
    $row[]=l('Edit',('discount-test-callback/'.$distributor_id) ,array('html' => TRUE, 'attributes' => array('class' => "ctools-use-modal")));    
    $rows[] = $row;
  }
  
  $output = theme('table', array('header' => $header, 'rows' => $rows)). theme('pager');
 
  return $output;
}

/**
 *function to get records by distributor_id
 */
function get_list_by_distributor_id($distributor_id){
  $query = db_select('ebay_books_discount','ebd');
  $query->fields('ebd',array('publisher_name','category_id','discount'));
  $query->condition('distributor_id',$distributor_id, '=');  
  $query = $query->extend('PagerDefault')->limit(10); 
  $isbn_redsn_dist = $query->execute()->fetchAll();
  return $isbn_redsn_dist;  
}

