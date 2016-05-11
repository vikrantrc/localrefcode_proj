<?php



/**
 * Display and assign sales orders 
 * Using Quick tab custom code
 */
function sales_auto_assign_order_reassignment(){
 drupal_add_css(drupal_get_path('module', 'sales') . '/sales.css');
 drupal_add_js(drupal_get_path('module','sales').'/sales.js'); 
 
 //Fetch all distributor who has assigned order
 if($distributor_tabs = sales_get_all_order_assigned_tabs()){ 
    
    foreach($distributor_tabs as $tabs_id){
        $distributor_id = $tabs_id->distrbutor_id;
        //Get distributor name by id
        $title = stock_get_distributor_name_by_id($distributor_id) ;
        $title =  ($title == '0') ? 'Market': $title;
        
        $mytabs[] = array(
          'title'   => $title,
          //'contents' => sales_show_table_structure_assign_orders($distributor_id),
          'weight'  => 0,
          'teaser' => FALSE,
          'hide_title' => TRUE,
          'path' => 'sales/order/get_assignment/'.$distributor_id,
          'type' => 'callback',
        );       
    }
    
    $tabs = $mytabs;
    $quicktabs_options = array(
       'style'   => 'Navlist',
       'ajax'    => TRUE,
       'hide_empty_tabs' => TRUE,
       //'default_tab' => $title,       
    );
    
    $quicktabs_name = 'dynamic_quicktabs_menu';
    $quicktabs = quicktabs_build_quicktabs($quicktabs_name, $quicktabs_options, $tabs);
    //return '<div id="assignment_quicktabs">'.$quicktabs.'</div>';
    return drupal_render($quicktabs);
  }//End If
}

/**
 * Implements callback of sales/order/reassign/ajax
 * 
 * Reasssigning the Book to another source.
 */ 
function sales_isbn_reassing(){
    $distributor = $_POST['distibutor'];//distributor_A id
    $isbn = $_POST['isbn'];
    $assign_to_info = $_POST['assignto'];
    $order_assigned_id= $_POST['assigned_order_id'];
    $price= $_POST['bookprice'];
    $order_id= $_POST['order_id'];
    $update_set_values = array('distrbutor_id' => $assign_to_info);
    $available_quantity = $_POST['availquantity'];//Quantity available with B
    $required_quantity = $_POST['requirequantity'];//Quantity to be assigned by A to B
    $remaining_quantity_with_A= abs($available_quantity-$required_quantity);//A q - Bq
    $distrb_b=$_POST['distrb_b'];//distributor_B id
    $insert_set_values_condition1 =array('order_id' => $order_id,
                                        'distrbutor_id' => $distrb_b,
                                        'isbn' => $isbn,
                                        'qty' => $available_quantity,
                                        'price'=>$price,
                                        'status' =>'assigned');
   
   $update_set_values_condition1 = array('qty' => $remaining_quantity_with_A );
   $res= check_available_isbn($isbn,$distrb_b);
   if(empty($res)){               
        insert_and_update($insert_set_values_condition1,$update_set_values_condition1,$distributor,$order_assigned_id);
    }        
     
    //print_r($order_assigned_id);die;
    else{ 

    }
}

/**
 * Ftech all distributors Name for assiged orders
 */ 
function sales_get_all_order_assigned_tabs(){
  $query = db_select('ebay_books_order_assigned','s'); 
  $query->condition('s.status', 'assigned', '='); 
  $query->fields('s',array('distrbutor_id'));
  $query->groupBy('s.distrbutor_id');
  $assignedto = $query->execute()->fetchAll();  
  return $assignedto;
}

/**
 * List all assign orders to the distributor
 */
function sales_show_table_structure_assign_orders($distributor_id){
  
    $header = array('ISBN', 'QTY', 'TITLE', 'PUBLISHER', 'REASSIGN TO', '');
    $data = sales_get_distributor_vise_orders($distributor_id);
    $rows = array();
    
    // Distributoe vise Information 
    foreach($data as $id => $book_info){
        $args = '';
        $options = sales_get_reassign_options($book_info->isbn,$distributor_id,$book_info->qty,$book_info->price,$book_info->order_id);
        $order_assigned_id=$book_info->order_assigned_id;
        $row = array();
        $row[] = $book_info->isbn;
        $row[] = $book_info->qty;
        $row[] = isset($book_info->title) ? $book_info->title : 'NA';
        $row[] = isset($book_info->publisher) ? $book_info->publisher : 'NA';
        
       /* foreach($options as $key => $option){
            $args .= '<option value="'.$key.'" qty="'.$qty.'">'.$option.'</option>';
        }*/
        
        $row[] = '<select id = "select_'.$distributor_id.'_'.$book_info->isbn.'">' . $options . '</select>';
        $row[] = '<button id = "'. $distributor_id . "_" . $book_info->isbn .'_'.$order_assigned_id.'" class = "reassign"> Add </button>';
        $rows[] = $row;
        
    }
     return '<div id = "distributor_'.$distributor_id.'">' . theme('table', array('header' => $header, 'rows' => $rows)) . '</div>';
}



function sales_show_table_structure_assign_orders_submit($form,&$form_state){
    echo "()()";
    print_r($form_state['values']); die;
}


/**
 * Fetch all records assigned to this distributor.
 * From table : sells_records_items, and : ebay_books_order_assigned
 * 
 */
function sales_get_distributor_vise_orders($distributor_id){
    $query = db_select('ebay_books_order_assigned','oa');
    //$query->leftjoin('sells_records_items','i',' i.custom_label = oa.isbn ');//JOIN isbn with temp table
    $query->fields('oa',array('isbn','order_assigned_id','qty','price','order_id'));//order_assigned_id
    $query->leftjoin('catalog', 'c', 'c.isbn13 = oa.isbn'); //JOIN isbm with catalog
    //$query->fields('i',array('custom_label','item_title'));//publisher
    $query->fields('c',array('publisher','title'));//publisher
    $query->condition('oa.distrbutor_id', $distributor_id); 
    $query->addExpression('SUM(oa.qty)', 'qty');
    $query->groupby('oa.isbn');
    $results = $query->execute()->fetchAll();
    return $results;
    }
