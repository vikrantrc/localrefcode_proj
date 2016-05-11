<?php 

/**
  *  Save Or Update table as needed
  
 function catalog_merge_records($catalog_xls_info, $isbn13){   
    $catalog_save= $catalog_xls_info;	//array of fields and it's values
	$catalog_update = $catalog_xls_info; //array of fields and it's values
	
	$catalog_save['created'] = time();
	$catalog_update['changed'] = time();
	
    //array_push($catalog_save, 'created' => time());	
	//array_push($catalog_update, "'changed'=> time()");
	
	$res = db_merge('catalog')
	  ->key(array('isbn13' => $isbn13))	 
	   ->insertFields($catalog_save)
       ->updateFields($catalog_update)	  
	  ->execute();	
   return $res;	  
 }
 */
 
 
 
 
 /**
  *  Save catalog records
  * 
  */
 function catalog_save_records($table_name,$catalog_xls_info){
     //dpr($catalog_xls_info); die;
    try{
    $rw = db_insert($table_name)
	->fields($catalog_xls_info)
	->execute();        
   	
    }catch(exception $e){
     print_r($catalog_xls_info);
     watchdog('DB Error',print_r($e));
    }
     return true; 
 }
 

 
 
 /**
  *  Update catalog records
  *
  */ 
 function catalog_update_records($catalog_xls_data){
    //dpr($catalog_xls_data);     
    // watchdog('before','From: %from', array('%from', print_r($catalog_xls_data)));
    foreach($catalog_xls_data as $cat_key => $cat_values){
      if(empty($cat_values) || trim($cat_values) == '' || $cat_values == '-'){
        
      }else{       
        $catalog_xls_info[$cat_key] = $cat_values;
      }
    }          
   //dpr($catalog_xls_info);die;
   // watchdog('after','From: %from', array('%from', print_r($catalog_xls_info)));      
   try{
   db_update('catalog')
   ->fields($catalog_xls_info)
   ->condition('isbn13', $catalog_xls_info['isbn13'], '=')
   ->execute();
    }catch(exception $e){
        watchdog('DB Update','From: %from', array('%from', print_r($e)));  
        dpr($catalog_xls_info);
    }	
  } 
 
 /**
  * Delete nodes of contnet type
  * @parm
  * $type is content type name of which all nodes to be delete
  */
 function catalog_delete_error_nodes($type){
    $error_nid = catalog_get_error_nid_by_isbn($type);
	//print_r($error_nid); die;
    //Delete all node related to contnet type catalog_error_node
    foreach($error_nid as $error_nid_value){
	  node_delete($error_nid_value->nid);  
    } 
  }
 
/**
 * Find the ISBN number is exist or not as catalog node.
 *
 * @param $isbn
 *   An ISBN uniqu book number 
 * 
 * @return
 *   nid if ISBN is alredy present as catalog node.
 *
 * @todo
 *  Nothing
 */
function catalog_get_nid_by_isbn($isbn) {    
  $query = db_select('catalog','c');
  $query->condition('c.isbn13', $isbn, '=');
  $query->fields('c',array('isbn13'));
  //dpq($query);
  $nid_reds = $query->execute()->fetchField();  
  return $nid_reds;
}


/**
 * Find the ISBN number is exist or not as catalog node.
 *
 * @param $isbn
 *   An ISBN uniqu book number 
 * 
 * @return
 *   nid if ISBN is alredy present as catalog error node .
 *
 * @todo
 *  Nothing
 */
function catalog_get_error_nid_by_isbn($type = 'catalog_error_node') { 
  $query = db_select('node','n'); 
  $query->condition('n.type', $type, '='); 
  $query->fields('n',array('nid'));
  $nid_reds = $query->execute()->fetchAll();  
  return $nid_reds;
}


/**
 * Save all error messages by row vise 
 *
 
function catalog_save_temp_error_messages($error_info){ 
  if(count($error_info) > 0){
	 // Empty table
	  $result = db_truncate('catalog_temp_error_message')->execute();
	 // Save  new error messages
	  $query = db_insert('catalog_temp_error_message')->fields(array('row_number', 'message'));
	  foreach ($error_info as $row => $type) {
		$query->values(array(
		  'row_number' => NULL,
		  'message' => serialize($type),     
		));
	  }
	  $query->execute();
  }
}*/

/**
 *  fetch all rows of error messages
 */
function catalog_get_error_records(){
 $select = db_select('catalog_error_details', 'm')
    ->fields('m'); 
   $error_messages = $select->execute()->fetchAll();  
  
   return $error_messages;
}




/**
 * Search Inputed filter and show result, Downlaod XLS file with result
 *
 */
function catalog_serach_filtered_input($date_from, $date_to, $inputed_filter = array(), $filter_extra_info = array(), $date_check){ 
  $query = db_select('catalog','c');
  $query->fields('c');  
  // $option_list = array('Title','Author 1','Category/ genre','eBay category ID 1','Publisher','Seller Id');
  //User filter input values
  foreach($inputed_filter as $filt_key => $inputed_filter_values){
    
    $option_selected = $inputed_filter_values['option_selected'][$filt_key];
	$input_value = $inputed_filter_values['input_value'][$filt_key];
	
	switch($option_selected){
	  case 'Title':  	    
	     $query->condition('title', "%$input_value%", 'LIKE'); 	     
	    continue;
		
	  case 'ISBN13': 	  	    
	     $query->condition('isbn13', "%$input_value%", 'LIKE'); 	     
	    continue;
      
      case 'Author 1':  	    
	     $query->condition('author1', "%$input_value%", 'LIKE'); 
	     continue;  
		
      case 'Category/ genre': 	    
	    $query->condition('category_genre', "%$input_value%", 'LIKE'); 	 
	    continue;
		
	  case 'eBay category ID 1': 	    
	    $query->condition('ebay_category_id_1', "%$input_value%", 'LIKE'); 	  
	    continue;
  	 
      case 'Publisher': 	    
	    $query->condition('publisher', "%$input_value%", 'LIKE'); 	 
	    continue;
   	  
      case 'Seller Id': 	    
	    $query->condition('seller_id', "%$input_value%", 'LIKE'); 	   
	    continue; 
   	}  //END OF Switch
   
  } //END of For Loop   
	
   /* case 'ISBN': 	  	    
	     $query->condition('isbn', "%$input_value%", 'LIKE'); 	      
	    continue;	  
	  
	  case 'Author 1':  	    
	     $query->condition('author1', "%$input_value%", 'LIKE'); 
	     continue;  
	   
	  case 'Author 2': 	  
	    $query->condition('author2', "%$input_value%", 'LIKE'); 
	    continue;
		
	  case 'Author 3': 	  
	    $query->condition('author3', "%$input_value%", 'LIKE'); 
	    continue;
		
	  case 'Author 4': 	  
	    $query->condition('author4', "%$input_value%", 'LIKE'); 
	    continue;
	  case 'Author 5': 	  
	    $query->condition('author5', "%$input_value%", 'LIKE'); 
	    continue;
	  
	  case 'Author 6': 	  
	    $query->condition('author6', "%$input_value%", 'LIKE'); 
	    continue;
		
	  case 'Author 7':	  
	    $query->condition('author7', "%$input_value%", 'LIKE'); 	  
	    continue;
	  	
	  case 'Author 8': 
	    $query->condition('author8', "%$input_value%", 'LIKE'); 
	    continue;
	  	
	  case 'Author 9': 
	    $query->condition('author9', "%$input_value%", 'LIKE'); 	   	  
	    continue;
	  
	  case 'Author 10': 
	    $query->condition('author10', "%$input_value%", 'LIKE'); 
	    continue;
		
	  case 'Author Introduction': 
	    
	    $query->condition('author10', "%$input_value%", 'LIKE'); 
	   
	    continue;
	  
	 case 'Book Image': 
	    $query->condition('book_image', "%$input_value%", 'LIKE'); 
	    continue;
	 
	 case 'Book description': 
	    
	    $query->condition('book_description', "%$input_value%", 'LIKE'); 	    
	    continue;
	  	
	 case 'Book reviews': 
	    
	    $query->condition('book_reviews', "%$input_value%", 'LIKE'); 	  
	    continue;
	   
	 case 'Category/ genre': 	    
	    $query->condition('category_genre', "%$input_value%", 'LIKE'); 	 
	    continue;
		
	 case 'eBay category ID 1': 	    
	    $query->condition('ebay_category_id_1', "%$input_value%", 'LIKE'); 	  
	    continue;		
		
	case 'eBay category ID 2': 
	    $query->condition('ebay_category_id_2', "%$input_value%", 'LIKE'); 
	    continue;	
	  		
	case 'MRP (INR)': 	   
	    $query->condition('mrp_inr', "%$input_value%", 'LIKE'); 	
	    continue;	
		
	case 'Number of pages': 	    
	    $query->condition('number_of_pages', "%$input_value%", 'LIKE'); 	  
	    continue;
		
	case 'Published date/ Edition': 	    
	    $query->condition('published_date_edition', "%$input_value%", 'LIKE'); 	  
	    continue;
		
	case 'Publisher': 	    
	    $query->condition('publisher', "%$input_value%", 'LIKE'); 	 
	    continue;
		
	case 'Type - Hardcover/ Paperback': 		
	    $query->condition('type_hardcover_paperback', "%$input_value%", 'LIKE'); 	  
	    continue;		
	case 'Seller Id': 	    
	    $query->condition('seller_id', "%$input_value%", 'LIKE'); 	   
	    continue;
	    		
	case 'Edition': 	    
	    $query->condition('edition', "%$input_value%", 'LIKE'); 	  
	    continue;		
	case 'Rating': 	   
	    $query->condition('rating', "%$input_value%", 'LIKE'); 	   
	    continue;
	case 'Meta Keywords': 	   
	    $query->condition('meta_keywords', "%$input_value%", 'LIKE');	   
	    continue;
		
	case 'Meta  Description': 	   
	    $query->condition('meta_description', "%$input_value%", 'LIKE');       
	    continue;
   */
   
   

  
   //Handler for Extra inputed multiple ISBN    
   //print_r($filter_extra_info); die;
  
   if($filter_extra_info){   
    // for time being directly handeling ISBN 13
    foreach($filter_extra_info as $ext_key => $ext_key_value){
        if($ext_key_value){           
            $ext_key_value=  str_replace("\r","",$ext_key_value);
            $exlpolded_isbn =  explode("\n",$ext_key_value); 
            //print_r($exlpolded_isbn);die;           
            $query->condition('isbn13', $exlpolded_isbn, 'IN'); 
        }        
      
    }//End of extra for loop       
   } //End of extra if      
  //$date_checked_value = array('date_filter_set' => $form_state['values']['date_check'], 'date_operation' => $form_state['values']['date_events_op']);
  
  //Check for date field if supplied
  if($date_check['date_filter_set']){
    if($date_check['date_operation'] == 'updated'){        
        $date_op = 'changed';
    }else{
        $date_op = 'created';
    }
    
	$query->condition('c.'.$date_op, array($date_from, $date_to), 'BETWEEN');
  }
  
  //echo "***"; 
  
  $nid_reds= $query->execute()->fetchAll();
 /// dpr($nid_reds); die;
  return $nid_reds; //catalog_serach_filtered_input
}
