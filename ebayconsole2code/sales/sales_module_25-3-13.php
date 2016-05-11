<?php    

require_once ('sales_page.inc');
    
/**
 * Implements hook_permissions().
 */
function sales_permission() {
  return array(
    'sales catagory managament' => array(
      'title' => t('Sales catagory  managament option'),     
    ),
  );
}


/**
 * Implements hook_menu().
 */
function sales_menu() {
  $items['sales/upload'] = array(
    'title' => 'Import Sales File',
    'description' => 'Sales File Upload',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('sales_file_upload'),
    'access arguments' => array('sales catagory managament'),   
  );
 $items['sales/order/assignment'] = array(
    'title' => 'Order Assignment',
    'description' => 'Order Assignment',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('sales_order_assignment'),
    'access arguments' => array('sales catagory managament'),   
  );
  
  $items['sales/order/assign'] = array(
    'title' => 'Check auto assign orders',
    'description' => 'Executive can check and assign auto assigned orders as per convenience.',
    'page callback' => 'sales_auto_assign_order_reassignment',
    'page arguments' => array('sales_auto_assign_order_reassignment'),
    'access arguments' => array('sales catagory managament'), 
    'file' => 'sales_page.inc' ,
  );
  
  $items['sales/order/reassign/ajax'] = array(
    'title' => 'ajax reassign',
    'description' => 'Executive can check and assign auto assigned orders as per convenience.',
    'page callback' => 'sales_isbn_reassing',
    'access arguments' => array('sales catagory managament'), 
    'file' => 'sales_page.inc' ,
  );
  
  $items['sales/order/get_assignment/%'] = array(
    'title' => 'Order Assignment', 
    'page callback' => 'sales_show_table_structure_assign_orders',
    'access arguments' => array('sales catagory managament'), 
    'file' => 'sales_page.inc' ,
    'type' => MENU_CALLBACK,
    'page arguments' => array(3),
  );
  $items['sales/order/get_picklist']=array(
    'title'=>'Generate Pickuplist',
    'description' => 'generate pickuplist',
    'page callback' => 'drupal_get_form',
    'page arguments' => array('sales_generate_pickup_list'),  
    'access arguments'=>array('sales catagory management'),
    );
  $items['sales/invoice/next'] = array(
  'title' => 'Invoicing', 
  'page callback' => 'sales_invoice_listing',
  'access arguments' => array('sales catagory managament'), 
  'file' => 'sales_page.inc' ,
  'type' => MENU_CALLBACK,
  );
	$items['sales/order/invoice/download/%/%'] = array(
  'title' => 'Invoicing', 
  'page callback' => 'sales_invoice_download',
  'access arguments' => array('sales catagory managament'), 
  'file' => 'sales_page.inc' ,
  'type' => MENU_CALLBACK,
	'page arguments' => array(4,5),
 );
 	$items['sales/order/invoice/search'] = array(
	'title' => 'Search Invoice', 
	'page callback' => 'sales_invoice_search',
	'page callback' => 'drupal_get_form',
	'page arguments' => array('sales_invoice_search'),
	'access arguments' => array('sales catagory managament'), 
	'type' => MENU_CALLBACK,
 
 );
	
	
  return $items;
 }
 
/**
 * 
 */
function sales_init(){
    //drupal_add_js(drupal_get_path('module','sales').'/jquery.ddslick.js') ;   
    drupal_add_js(drupal_get_path('module','sales').'/sales.js'); 
}

/**
 * Implements callback of sales/upload
 */
 function sales_file_upload(){
	$form['sales_report'] = array(
		'#type' => 'file',
		'#title' => t('Select Sales file'),
		'#size' => 22,
    //'#required' => TRUE,
    '#description' => '<br/>#Note : Upload Sales.csv file only',
	);
  
  
	$form['read_sales_report'] = array(
		'#type' => 'submit', 
		'#value' => t('Upload'),
		
	);
	return $form;
 }
 
 function sales_invoice_search(){
		$form['invoice_no'] = array(
		'#type' => 'textfield', 
		'#title' => 'Enter Invoice Number',
		'#required'=> TRUE,
		);
		$form['invoice_no_submit'] = array(
		'#type' => 'submit', 
		'#value' => t('Go'),	
		'#ajax' => array(
		'callback' => 'invoice_search_preview',
		'wrapper' => 'invoice_search_preview_area',
		'method' => 'html',
		'effect' => 'fade',
		),
		);
		$form['wrapper'] = array(
		'#prefix'  => '<div id="invoice_search_preview_area">',
		'#suffix'  => '</div>',
		);

    return $form;
 }
 //Call back of search invoice
 function invoice_search_preview($form,&$form_state){
  $header = array();
  $default_value = array();
  $IsCheckbox = true;
  $header = array('isbn'=>'ISBN', 'qty'=>'Quantity', 'order_id'=>'Order Id', 'price'=>'Price');
  if($IsCheckbox){
        $default_value[0] = true;
        $default_value[1] = false;
    }else{
     $default_value = '0';
    }
  $invoice_no = trim($form_state['values']['invoice_no']);
  $query = db_select('ebay_books_order_assigned','oa');
  $query->fields('oa',array('isbn','qty','price','order_id','order_assigned_id')); 
	$query->condition('invoice_no',$invoice_no);
  $results = $query->execute()->fetchAll();
  
	foreach($results as $invoice_list){
    /*$row = array();
    $isbn= $invoice_list['isbn'];
    $qty = $invoice_list['qty'];
    $order_id = $invoice_list['order_id'];
    $price = $invoice_list['price'];
    $options[$row['isbn']] = $row;*/
    
   }
   

	 
	// if(count($rows)>0) {
	  $form=array();
       $form['myselector'] = array (
        '#type' => 'tableselect',
        '#title' => 'My Selector',
        '#header' => $header,
        '#options' => $rows,
        /*'#multiple' => $IsCheckbox,
        '#default_value' => $default_value*/
    );
  //	$tabl = theme('table',array('header' => $header,'rows'=>$rows));
	// }
   if(!$rows){
          $form['no_rec_promo'] = array ( 
         '#markup' => "No records found",
     );
    }
   
  // else {
	//	$tabl = 'Please enter correct invoice number. It doesnot exist in the table :(';
	// }
	
	$form = array();
	
			 $options = array(
      array(
        'title' => 'How to Learn Drupal',
        'content_type' => 'Article',
        'status' => 'published',
        '#attributes' => array('class' => array('article-row')),
      ),
      array(
        'title' => 'Privacy Policy',
        'content_type' => 'Page',
        'status' => 'published',
        '#attributes' => array('class' => array('page-row')),
      ),
    );
    $header = array(
      'title' => t('Title'),
      'content_type' => t('Content type'),
      'status' => t('Status'),
    );
		$default_value = array();
    $form['taddble'] = array(
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => $options,
		  '#default_value' => $default_value
      //'#empty' => t('No content available.'),
    );
	
	  return $form;
//	return $tabl;
 }
 function sales_order_assignment(){
//	drupal_add_css(drupal_get_path('module', 'stock') . "/stock.css");
  $not_assigned = sales_get_no_unassigned_items();
  $form['markup'] = array(
	 '#type' => 'markup', 
	 '#markup' => '<h2>'.$not_assigned.' items still to be assigned</h2><p><h2>Please click the below button to process the order</h2>',	
  );
  $form['order_assign_hidden'] = array(
	 '#type' => 'hidden', 
	 '#value' => 10,	
  );
  
  $form['order_assign'] = array(
	 '#type' => 'submit', 
	 '#value' => t('Order Porcessing'),	
  );
  return $form;
}
//generate pickup_list
 function sales_generate_pickup_list(){
 $form['markup']=array(
 '#type' => 'markup',
 '#markup' => '<h2>Please click the below button to download Pickup List</h2>',
 );
 $form['download_pickuplist']=array(
 '#type' => 'submit',
 '#value' => t('Download Pickuplist'),
 ); 
 return $form;
}
 
/**
 * Implements form_validate
 */ 
 function sales_file_upload_validate($form, &$form_state){
  // dpr($form_state); die;
	$file = file_save_upload('sales_report', array('file_validate_extensions' => array('csv'),));
	if ($file) {
      if ($file = file_move($file, 'public://')) {
        $form_state['values']['file'] = $file;
      }else {
        form_set_error('file', t('Failed to write the uploaded file the site\'s file folder.'));
      }    
    
    //Check column count for csv.    
     $file = $form_state['values']['file'];
	 $file->status = FILE_STATUS_PERMANENT;
	 $filepath = drupal_realpath($file->uri);
     $csv = readCSV($filepath);
     $col_count = count($csv[0]);
     
     if($col_count != 37){        
         form_set_error('file', t('Uploaded file column count does not matched, can\'t process it.'));
     }
     
     //Check for ISBN is formated or Not
     
     	$file = $form_state['values']['file'];
    	$file->status = FILE_STATUS_PERMANENT;
    	$filepath = drupal_realpath($file->uri);
       
         //Custom function to read csv file.   
        $csv = readCSV($filepath);
        $csv_col_count = count($csv[0]);
        
         //print_r($csv); die;
         //loop it in foreach
        $s_count = 0;
        foreach($csv as $cky => $csv_values){         
           //Check to start from row second.
           if($cky > 0 && !empty($csv_values[0])){       
             $isbn_val =  $csv_values[13];// Custom label i.ISBN
           
             if(stristr($isbn_val, '+')){
                 form_set_error('file', t('Row number '. $cky.' column custom lable ISBN value is invalid.'));
             }             
            }//END of Inner If            
        }//END of foreach loop    
    }// END of check ifFile ixist
 }
 
 /**
 * Implements form_submit
 */
 function sales_file_upload_submit($form, &$form_state){
	$file = $form_state['values']['file'];
	$file->status = FILE_STATUS_PERMANENT;
	$filepath = drupal_realpath($file->uri);
   
     //Custom function to read csv file.   
    $csv = readCSV($filepath);
    $csv_col_count = count($csv[0]);
    
     //print_r($csv); die;
     //loop it in foreach
    $s_count = 0;
    foreach($csv as $cky => $csv_values){
       // for($s=0; $s<$csv_col_count;$s++){
       // $csv_fetch_info[$cky][] =  $csv_values[$s]; 
        //echo "**(*(*()))";
       //Check to start from row second.
       if($cky > 0 && !empty($csv_values[0])){          
        
        // Will take only 11 column for each row.
        $sales_order_rec = array(
                            'sales_record_no' => check_plain($csv_values[0]),
                            'user_id' => check_plain($csv_values[1]),                               
                            'buyer_full_name' => check_plain($csv_values[2]),
                            'buyer_phone' => check_plain($csv_values[3]),
                            'buyer_email' =>  check_plain($csv_values[4]),
                            
                            'buyer_address_1' => check_plain($csv_values[5]),
                            'buyer_address_2' => check_plain($csv_values[6]),
                            'buyer_city' => check_plain($csv_values[7]),                               
                            'buyer_state' => check_plain($csv_values[8]),
                            'buyer_pincode' => check_plain($csv_values[9]),
                            'buyer_country' =>  check_plain($csv_values[10]),
                            
                            'total_price' => check_plain(floatval(str_replace("Rs.","",$csv_values[19]))),
                            'shipping_handlings' => check_plain(floatval(str_replace("Rs.","",$csv_values[16]))),//check_plain($csv_values[16]),
                            'insurance' => check_plain(floatval(str_replace("Rs.","",$csv_values[17]))),//check_plain($csv_values[17]),                               
                            'cash_on_delivery_fee' => empty($csv_values[18])  ? "00000.00" : check_plain(trim($csv_values[18])),
                            'shipped_on_date' => check_plain(intval($csv_values[24])),
                            'feedback_left' =>  check_plain($csv_values[25]),
                            
                            'feedback_received' => check_plain($csv_values[26]),
                            'notes_to_yourself' => check_plain($csv_values[27]),
                            'paypal_transaction_id' => check_plain($csv_values[28]),                               
                            'awb_number' => check_plain($csv_values[34]),
                            'cash_on_delivery_option' => check_plain($csv_values[30]),
                            'transaction_id' =>  check_plain($csv_values[31]),
                            
                            'record_uploaded_date' => time(),
                            'payment_method' => check_plain($csv_values[20]),
                            
                            'sale_date' => check_plain(strtotime($csv_values[21])),                               
                            'check_out_date' => check_plain(strtotime($csv_values[22])),
                            'pay_date' => check_plain(strtotime($csv_values[23])),                            
                            'shipping_service' => check_plain($csv_values[29]),
                            'order_id' => check_plain(intval($csv_values[32])),
                            'variation_details' => check_plain($csv_values[33]),
                            'courier_name' => check_plain($csv_values[35]),                               
                            'shipping_status' => check_plain(floatval(str_replace("Rs.","",$csv_values[16]))),//check_plain($csv_values[16]),
                            'order_status'=> 'queued',
                          
                           );
           // print_r($csv_values[0]);die;
        //Check order no is already exist?                 
        $sale_rec_id = sales_get_record_by_id(check_plain($csv_values[0]));
        
     
 
               
        //Save sales records
        if(!$sale_rec_id){
            
            $table = 'sales_records';  
            $res = ebay_books_save_records($table,$sales_order_rec);
            $s_count++;
        }   
           
  		if(!empty($csv_values[11])){
    		  //echo $csv_values[11]; echo "<br/>";
          //Recheck if same file upload again, so no duplicate records should upload
          $item_number =  sales_check_item_id($csv_values[11]);
           if(!$item_number){ 
      			     $sales_record_items = array(
                  									'sell_rec_id' =>NULL, //Auto increement.
                  									'sales_record_number' => check_plain($csv_values[0]),    //PK                           
                  									'item_number' => check_plain($csv_values[11]),
                  									'item_title' => check_plain($csv_values[12]),
                  									'custom_label' =>  check_plain($csv_values[13]),
                  									
                  									'quantity' => check_plain(intval($csv_values[14])),
                  									'source' => check_plain($csv_values[35]),
                  									'status' => 'queued',                               
                  									'remarks' => check_plain($csv_values[8]),
                  									'awb_no' => check_plain($csv_values[34]),
                  									'courrier_name' =>  check_plain($csv_values[35]),
                  									
                  									'shipping_status' => 0,
              								    	'sale_price' => check_plain(floatval(str_replace("Rs.","",$csv_values[15]))),//check_plain($csv_values[15]),
              								    	'ISBN' => check_plain(intval($csv_values[36])), 
                                                    'assignedto' => '',
  								                );
  								
              /// echo "&&&&&&&";
      		// dpr($sales_record_items); die;
      			$table = 'sells_records_items';                
      		  $res = ebay_books_save_records($table,$sales_record_items);
            
          }
        }           
             
     }//END of outer if loop.       
    
   // }//END inner for loop 
   
   }//END outer for loop 
    //dpr($sales_record_items); die;        
  // dpr($csv_fetch_info); die;
 //dpr($sales_record_items); die;
   drupal_set_message($s_count . ' Sales records uploaded successfully.');
}

/**
 * Get Sale records id if present
 */
function sales_get_record_by_id($sale_rec_id){
  $query = db_select('sales_records','s');
  $query->condition('s.sales_record_no', $sale_rec_id, '=');
  $query->fields('s',array('sales_record_no'));
  $sale_resp_id = $query->execute()->fetchField();  
  return $sale_resp_id;
} 

/**
 * Get Sale unique Item id  if present
 */
function sales_check_item_id($sale_item_id){
  $query = db_select('sells_records_items','s');
  $query->condition('s.item_number', $sale_rec_id, '=');
  $query->fields('s',array('sell_rec_id'));
  $sale_resp_id = $query->execute()->fetchField();  
  return $sale_resp_id;
} 

/**
 * Custom function to read csv file.
 */
function readCSV($csvFile){
 //Check if file  exist or not 
 if(file_exists($csvFile)){
	$file_handle = fopen($csvFile, 'r');
	while (!feof($file_handle) ) {
		$line_of_text[] = fgetcsv($file_handle, 1024);
	}   
	fclose($file_handle);
	return $line_of_text;
 }else{
    return false;
 }
}

/**
 * Helper function to give availability table 
 */
function stock_get_availability($isbn){
    
}

/**
 * Helper function to give assignment table
 */
function sales_create_isbn_table($isbn){
    $query = db_select("stock","s");
    $query->condition("s.isbn13",$isbn);
    $query->fields("s");
    $results = $query->orderBy('price')->orderBy('qty')->execute()->fetchAll();
    $rows = array();
    
    $header = array('isbn13'=>'ISBN13','qty'=>'QUANTITY','price');
    foreach($results as $result){
      $row = array();
      $row[] = $result->isbn13;
      $row[] = $result->qty;
      $row[] = $result->price;
      $row[] = $result->qty;
      $rows[] = $row;
    }
   
    $output = theme('table', array(
      'header' => $header,
      'rows' => $rows,
      'attributes' => array('class' => array('mytable'))
    ));
    return $output;
}


/**
 * Handling of pick up list download
 */
  function sales_generate_pickup_list_submit($form,&$form_state){
  $query= db_select('sells_records_items','pick');
  $query->condition('status','assigned');
  $query->fields('pick',array('sell_rec_id','item_title','custom_label','quantity'));
  $results = $query->execute()->fetchAll();
  $header = array('sell_rec_id','Title','ISBN','Quantity');
  foreach($results as $pick_list){
    $rows = array();
    $rows[]= $pick_list->sell_rec_id;   
    $rows[]= $pick_list->item_title; 
    $rows[]= $pick_list->custom_label;
    $rows[]= $pick_list->quantity;
    $xls_content_row .= implode("\t", array_values($rows)) . "\r\n";
    
  }//end of for
  $xls_content_header = implode("\t", array_values($header));
    	$xls_content = $xls_content_header."\n".$xls_content_row;
    	$filename = 'Stock_filter_list_'.date("d_m_Y"); 
    	header("Content-type: text/plain; charset=UTF-8");
    	header("Content-Disposition: attachment; filename=$filename");
    	header("Content-Type: application/vnd.ms-excel"); 
    	header("Pragma: no-cache");
    	header("Expires: 0");
    	print $xls_content;
    	exit();   
    	return TRUE;
 }

/**
 * Handling of auto assignment of Sells
 */
function sales_order_assignment_submit($form,&$form_state){
    
    $query = db_select('sells_records_items','i');
    $query->condition('status','queued');
    $query->fields('i',array('sell_rec_id','sales_record_number','custom_label','quantity','sale_price'));
    $results = $query->execute()->fetchAll();
    $count = 0;
    
    //Table name where we will keep sales order assigned an freezed
    $assigned_to_table = 'ebay_books_order_assigned';
    
    foreach($results as $result){
        $isbn = (float)$result->custom_label;
        $sell_rec_id = $result->sell_rec_id;
        $order_id = $result->sales_record_number;
        $sale_qty = $result->quantity;
        $sale_price = $result->sale_price;
        
      // echo "***". $isbn; 
        
        $qry = db_select('stock','s');
        $qry->join('field_data_field_source_rating','rating','s.source=rating.entity_id');
        $qry->fields('s',array('qty','source','isbn13'));
        $qry->fields('rating',array('field_source_rating_value'));
        $qry->condition('s.isbn13',$isbn);
        $qry->orderBy('rating.field_source_rating_value', 'DESC');
        $qry->orderBy('s.qty', 'DESC');
        $res1 = $qry->countQuery()->execute()->fetchField();
        $res = $qry->execute()->fetchAll();
    // print_r($res);die;
        $jj[]=$res;
      
        if(empty($res)){
       
            $q = db_update('sells_records_items')
                  ->fields(array(
                    'assignedto' => 0,
                    'status' => 'assigned',
                  ))
                  ->condition('sell_rec_id',$sell_rec_id)//
                  ->execute();
            
            //Save data in temp. sales order table     
            $input_array = array(
                              'order_id' => $order_id,
                              'distrbutor_id' => 0, //Assigned to Market
                              'isbn' => $isbn,
                              'qty' => $sale_qty,
                              'price' => $sale_price,
                              'status' => 'assigned'
                            );
            
            ebay_books_save_records($assigned_to_table,$input_array);
            $count++;
        }else{
            
                 
            
         // dpr($res); die;
        /*available*/
          //  if($res1 == 1){
               //only one source available
               if($res[0]->qty >=  $sale_qty){
                //book in stock
                $q = db_update('sells_records_items')
                      ->fields(array(
                        'assignedto' => $res[0]->source,
                        'status' => 'assigned',
                      ))
                      ->condition('sell_rec_id',$sell_rec_id)
                      ->execute();
                      
                //Save data in temp. sales order table     
                $input_array = array(
                                  'order_id' => $order_id,
                                  'distrbutor_id' => $res[0]->source,
                                  'isbn' => $isbn,
                                  'qty' => $sale_qty,
                                  'price' => $sale_price,
                                  'status' => 'assigned'
                                );
                
                ebay_books_save_records($assigned_to_table,$input_array);
                
               }else{
                //out of stock.. from market
                $q = db_update('sells_records_items')
                      ->fields(array(
                        'assignedto' => 0,
                        'status' => 'assigned',
                      ))
                      ->condition('sell_rec_id',$sell_rec_id)
                      ->execute();
                
                //Save data in temp. sales order table     
                $input_array = array(
                                  'order_id' => $order_id,
                                  'distrbutor_id' => 0,
                                  'isbn' => $isbn,
                                  'qty' => $sale_qty,
                                  'price' => $sale_price,
                                  'status' => 'assigned'
                                );
                
                ebay_books_save_records($assigned_to_table,$input_array);
              }
            //}else{
//                //multiple stocks available
//                
            } 
        }
       
 }
 

function sales_get_no_unassigned_items(){
    $query = db_select('sells_records_items','i');
	$query->condition('i.status','queued');
	$result = $query->countQuery()->execute()->fetchField();
    return $result;
}

/**
 * distributor Select list options to order assignment
 * 
 * 
 */
function sales_get_reassign_options($isbn,$distributor_id,$qty,$price,$order_id){
    $query = db_select('stock','s');
    $query->condition('s.isbn13',$isbn);
    $query->condition('s.source',$distributor_id,'!=');
    $query->condition('s.qty',0,'!=');
    $query->fields('s');
    $results = $query->execute()->fetchAll();
    //$output = array();
    
    $output = '';
    $output .= '<option value="0" >distributor - qty</option>';
    
    foreach($results as $result){
        $dist = stock_get_distributor_name_by_id($result->source);
        $dist_id_b=$result->source;
        //$output[$result->source] = $dist . ' - ' . $result->qty;
        $output .= '<option value="'.$result->source.'" reqqty="'.$qty.'" avlqty="'.$result->qty.'" bkprice="'.$price.'"distr_b="'.$dist_id_b.'"order_id="'.$order_id.'">'.$dist . ' - ' . $result->qty.'</option>';
    }
     
    
    return $output;
}
