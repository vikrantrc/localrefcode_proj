<?php
/**
 * Display and assign sales orders 
 * Using Quick tab custom code
 */
function sales_auto_assign_order_reassignment(){
 drupal_add_css(drupal_get_path('module', 'sales') . '/sales.css');
 
 if($distributor_tabs = sales_get_all_order_assigned_tabs()){
        //print_r($distributor_tabs);die;
      foreach($distributor_tabs as $tabs_id){
        //echo "**" . $tabs_id->assignedto;
        $distributor_id = $tabs_id->assignedto;
        $title = stock_get_distributor_name_by_id($distributor_id) ;
        $title =  ($title == '0') ? 'Market': $title;
        $mytabs[] = array(
          'title'   => $title,
          //'type'    => 'callback',
           'contents' => drupal_get_form('sales_show_table_structure_assign_orders',$distributor_id),// Creates a table structur
          // 'path' => 'sales/order/get_assignment/'.$distributor_id,
           'weight'  => 0,
        );       
       } 
      // dpr($mytabs); die;
       
    $tabs = $mytabs;

    // Configure the QuickTabs options
    $quicktabs_options = array(
       'style'   => 'Navlist',
       'ajax'    => TRUE,
    );
    
    $quicktabs_name = 'dynamic_quicktabs_menu';
    $quicktabs = quicktabs_build_quicktabs($quicktabs_name, $quicktabs_options, $tabs);
    return $quicktabs;
  }//End If
}


/**
 * Ftech all distributors Name for assiged orders
 * 
 */ 
function sales_get_all_order_assigned_tabs(){
  $query = db_select('sells_records_items','s'); 
  $query->condition('s.status', 'assigned', '='); 
  $query->fields('s',array('assignedto'));
  $query->groupBy('s.assignedto');
  $assignedto = $query->execute()->fetchAll();  
  return $assignedto;
}

/**
 * List all assign orders to the distributor
 */
function sales_show_table_structure_assign_orders($form,&$form_state,$distributor_id){
    
     $form = array();
    $form['order_info']['order_sheet'] = array(	  
      '#type' => 'fieldset',
	  '#title' => 'Check and rearrange orders',
	  '#collapsible' => FALSE,
	  '#collapsed' => FALSE,
      '#prefix' => '<div id="order-distributor-wrapper">',
      '#suffix' => '</div>',
      '#weight' => 9,
    );
        
    $form['order_info']['order_sheet']['show_table'] = array(	 
      '#prefix' => '<table border="1" align="center" padding="2" >',
      '#suffix' => '</table>', 
    );
    
    
    $form['order_info']['order_sheet']['show_table']['header'] = array(
              '#prefix' => '<tr class = "even"> 
                           <th>ISBN</th> <th>QTY</th> 
                           <th>Title</th> <th>Publisher</th>
                           <th>Reassign to</th> <th>Operation</th>',
              '#suffix' => '</tr>',
     );
     
     $data = sales_get_distributor_vise_orders($distributor_id);
     //dpr($data);die;
     foreach($data as $id => $book_info){
        //dpr($book_info);die; 
            $form['order_info']['order_sheet']['show_table']['header'][$id]['show_row_'.$book_info->custom_label] = array(
              '#prefix' => '<tr class = "even">',
              '#suffix' => '</tr>',
            );
        
         //foreach($sku_final_data as $sid_key => $subm_values){             
            $form['order_info']['order_sheet']['show_table']['header'][$id]['show_row_'.$book_info->custom_label][$id]['isbn']= array 
            (     
              '#type' => 'markup',           
              '#markup' => $book_info->custom_label,
              '#prefix' => '<td>',
              '#suffix' => '</td>',
            );
            
            $form['order_info']['order_sheet']['show_table']['header'][$id]['show_row_'.$book_info->custom_label][$id]['qty']= array 
            (     
                '#markup' => $book_info->count,    
                '#prefix' => '<td>',
                '#suffix' => '</td>',
            );
            
            $form['order_info']['order_sheet']['show_table']['header'][$id]['show_row_'.$book_info->custom_label][$id]['title']= array 
            (     
              //'#type' => 'textfield',           
              '#markup' => $book_info->item_title,
              '#prefix' => '<td>',
              '#suffix' => '</td>',
            );
            
            $form['order_info']['order_sheet']['show_table']['header'][$id]['show_row_'.$book_info->custom_label][$id]['publisher']= array 
            (     
                '#markup' => isset($book_info->publisher) ? $book_info->publisher : 'NA',    
                '#prefix' => '<td>',
                '#suffix' => '</td>',
            );
            
            $options = sales_get_reassign_options($book_info->custom_label,$distributor_id);
            
            $form['order_info']['order_sheet']['show_table']['header'][$id]['show_row_'.$book_info->custom_label][$book_info->custom_label]= array 
            (     
              '#type' => 'select',           
              '#options' =>  $options,
              '#prefix' => '<td>',
              '#suffix' => '</td>',
            );
            
            $form['order_info']['order_sheet']['show_table']['header'][$id]['show_row_'.$book_info->custom_label]['operation']= array 
            (     
                '#type' => 'submit', 
                '#value' => 'Add',    
                '#prefix' => '<td>',
                '#suffix' => '</td>',
            );
      }  
      //}           

    return $form;  
     
}



function sales_show_table_structure_assign_orders_submit($form,&$form_state){
    echo "()()";
    print_r($form_state['values']); die;
}


function sales_get_distributor_vise_orders($distributor_id){
    $query = db_select('sells_records_items','i');
    $query->leftjoin('catalog', 'c', 'c.isbn13 = i.custom_label'); //JOIN isbm with catalog
    $query->condition('i.status','assigned');
    $query->condition('i.assignedto',$distributor_id);
    $query->fields('i',array('custom_label','item_title'));//publisher
    $query->fields('c',array('publisher'));//publisher
    $query->addExpression('SUM(i.quantity)', 'count');
    $query->groupby('i.custom_label');
    //dpq($query);
    $results = $query->execute()->fetchAll();
    return $results;
}