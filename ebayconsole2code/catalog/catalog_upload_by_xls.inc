<?php

function catalog_upload_catalog_with_xls($form, &$form_state){  
  $file_cat_ref = drupal_get_path('module', 'catalog') . "/catalog_upload_example.xlsx";
	$form['catalog_upload'] = array(
		'#type' => 'file',
		'#title' => t('Choose a file'),
		'#title_display' => 'invisible',
		'#size' => 22,      
		'#description' => '#Note : You can upload up to fifty thousands records at a time.'."<br>". '<h3>To download catalog template file click ' .l('Here', $file_cat_ref).'</h3>',
		'#weight' => -10,
	);
    	
  $form['read_excel_file'] = array(
		'#type' => 'submit', 
		'#value' => t('Upload'),
 	  '#weight' => -9,
	);     
	return $form;
}


/**
  * Validate a catalog upload form
  */
 function catalog_upload_catalog_with_xls_validate($form, &$form_state){
    //print_r($_FILES);  die;
	$file = file_save_upload('catalog_upload', array('file_validate_extensions' => array('xls'),));
	require_once 'excel_reader2.php';
	
	if ($file) {
      if ($file = file_move($file, 'public://')) {
        $form_state['values']['file'] = $file;
      }else {
        form_set_error('file', t('Failed to write the uploaded file the site\'s file folder.'));
      }
    }
	
	$file=$form_state['values']['file'];
	$file->status = FILE_STATUS_PERMANENT;
	
	$filepath = drupal_realpath($file->uri);
	
	//print_r($filepath); die;
    $data = new Spreadsheet_Excel_Reader($filepath);
	  $numrows= count($data->sheets[0]['cells']);
    
    $numcols= $data->sheets[0]['numCols']; 
    
  //echo $numcols; 
	if($numcols != 31){
	  form_set_error('catalog_upload', 'Uploaded file doesn\'t match column count is .'. $numcols);
	} 
	
    if($numcols > 500000){
          form_set_error('catalog_upload', 'Uploaded file has more than fifty thousand records. Please upload it max fifty thousand records only.');
    }
	$failed = array();
	     
 }
 
    
 /**
  *  Handling catalog submition. 
  */ 
 function catalog_upload_catalog_with_xls_submit($form, &$form_state){
  //echo "***".	print_r($form_state['values']);die;
	require_once 'excel_reader2.php';
	global $user;
	
	//Delete all error nodes before new error nodes creation. 
	catalog_delete_error_nodes($type = 'catalog_error_node');	

 	$file = $form_state['values']['file'];
	$file->status = FILE_STATUS_PERMANENT;
	$filepath = drupal_realpath($file->uri);
  $data = new Spreadsheet_Excel_Reader($filepath);

  $numrows = count($data->sheets[0]['cells']);
  $numcols = $data->sheets[0]['numCols']; 
    
   // echo "*****" .$numrows; die;	
	
	$flag = 0; 	
	$update_cnt = $save = $broken = 0; 
    $output = array();
  // run through a rows here	
  for($row = 2; $row <= $numrows; $row++) {   
	   // run through a column here      
      for ($col = 1; $col <= $numcols; $col++) {
        $output[$row][$col] = $data->val($row, $col, $sheet = 0);
      } 	  

	//$newNode->title = $output[$row][2];   
    ########### Start Run time Validation ####################
	  $isbn13 = catalog_create_thirteen_digit_isbn($output[$row][2]); //ISBN13
    $isbn = $output[$row][3]; ////ISBN 
     
    
    $book_image = $output[$row][16];
    
   /* //This valdation for book image
    if(empty($book_image)){
            $book_image = $isbn13.'.jpg';            
    }else{        
        if(stristr($output[$row][16], '.jpg') === FALSE) {
            $book_image = $output[$row][16].'.jpg'; 
        }
    } */   
    ########### End Run time Validation ####################
    
   
   	$flag = array();
    
	//print_r($output[$row]);die;
	//Validate the xls information, here we are checking rowby row values
    $flag[$row] = catalog_check_xls_validation_with_xls($output[$row]);  
    
    //transliterate
    $trans_book_desc = mb_convert_encoding($output[$row][15],"HTML-ENTITIES",'utf8');
    
    //Calculate listing price
    $listing_price = stock_price_calculator($isbn13,floatval($output[$row][21]),check_plain($output[$row][24]));

    // Create an array to save or update catalog in db.
	$catalog_xls_info = array(
		'title'=> catalog_make_text_sentence_case( mb_convert_encoding($output[$row][1],"HTML-ENTITIES",'utf8')),// catalog_make_text_sentence_case(mb_convert_encoding($output[$row][1], 'Windows-1252', 'UTF-8')),//catalog_make_text_sentence_case($output[$row][1]),
		'isbn13'=> check_plain($isbn13),      //$isbn13
		'isbn'=>  check_plain($isbn),      //$isbn        
		'author1'=>  ucwords(mb_convert_encoding($output[$row][4],"HTML-ENTITIES",'utf8')),
		'author2'=> ucwords(check_plain($output[$row][5])),
		'author3'=>  ucwords(check_plain($output[$row][6])),
		'author4'=>  ucwords(check_plain($output[$row][7])),
		'author5'=>  ucwords(check_plain($output[$row][8])),
		'author6'=>  ucwords(check_plain($output[$row][9])),
		'author7'=>  ucwords(check_plain($output[$row][10])),
		'author8'=>  ucwords(check_plain($output[$row][11])),
		'author9'=>  ucwords(check_plain($output[$row][12])),
		'author10'=> ucwords(check_plain($output[$row][13])),
                
		'author_introduction'=>check_plain(empty($output[$row][14]) ? '-' : $output[$row][14]),
		'book_description'=> empty($output[$row][15]) ? '-' : $trans_book_desc,//iconv('Windows Latin 1','UTF-8',$output[$row][15]), //output[$row][15],
        //$output[$row][16]
        
		'book_image'=> $book_image,
		'book_reviews'=>check_plain($output[$row][17]),
         
		'category_genre'=>check_plain(str_replace("/",",",$output[$row][18])),
		'ebay_category_id_1'=>check_plain($output[$row][19]),
		'ebay_category_id_2'=>check_plain($output[$row][20]),
		'mrp_inr'=>floatval($output[$row][21]),
		'number_of_pages'=>$output[$row][22],
        
		'published_date_edition'=> empty($output[$row][23]) ? '-' : check_plain($output[$row][23]),
		'publisher'=>check_plain($output[$row][24]),
		'type_hardcover_paperback'=>check_plain($output[$row][25]),
		'seller_id'=> empty($output[$row][26]) ? '-' : check_plain($output[$row][26]),
        
		'edition'=>check_plain($output[$row][27]),
    'language'=> ucwords(check_plain($output[$row][28])),
		'rating'=>check_plain($output[$row][29]),
		'meta_keywords'=>check_plain($output[$row][30]),
		'meta_description'=> check_plain($output[$row][31]),
        'listing_price' => $listing_price,
   // 'imprint' =>  check_plain($output[$row][31]),
		);     
        
	 $catalog_xls_info['status'] = 1; 
	  
	  //print_r($flag); die;
		 
	 //Save error rows in another caontent type
	if($flag[$row]['status'] == 'False'){	
        //error_message
        $error_message = $flag[$row]['error_message'];
              
	    $catalog_error_details = array_merge($catalog_xls_info, array('catalog_temp_error_message' => $error_message));
       // print_r($catalog_error_details);die;
        
        if($broken == 0){
             //Empty "catalog_error_details" tabel before saveing new informations.
             $result = db_truncate('catalog_error_details')->execute();
        }       
        
        unset($catalog_error_details['listing_price']);
        
        $table_name = 'catalog_error_details';
	    catalog_save_records($table_name, $catalog_error_details); // save created time 
        
        //catalog_temp_error_message
	    ++$broken;	   
	}else{	
	   //Save/Update nid in catalog content type.
		$isbn_res = catalog_get_nid_by_isbn($isbn13);// How to get ISBN if we dont have ISBN in xls list?
		
		if($isbn_res){        
		  $catalog_xls_info['isbn13'] = $isbn13;
		  $catalog_xls_info['changed'] = time();//Save updated time
		  catalog_update_records($catalog_xls_info);	           
		  ++$update_cnt;//Keep updated record count		  
		}else{	 	
		  if($isbn13){ //Only ISBN having recors will creats
           //dpr($catalog_xls_info);die;$catalog_xls_info
            $catalog_xls_info['created'] = time();//Save updated time
            $table_name = 'catalog';
		      catalog_save_records($table_name, $catalog_xls_info); // save created time 
		    	++$save; //Keep newly saved record count 			
		  }
		}	
	}	
	
   }//End for loop	
	
	$xls_broken_rows = ($broken == 0) ? 0: 'Click <a href="catalog/broken/records"> Here </a> to download the: ' .$broken.' broken records.';
	
	drupal_set_message('Catalog total records were : '. ($numrows - 1)  .'<br/>New created catalogs are : '. $save
	                   .'<br/> Updated catalogs are : '. $update_cnt.'
					   <br/>');
     if($xls_broken_rows){
        drupal_set_message($xls_broken_rows,'error');
     }   
                          
 }
  
 
 /**
 * This will check each row and column info and validate it.
 *
 * @param $xls_data
 *  Is a catalog xls having catalog information in rows and column  
 * 
 * @return
 *   $flag is true if no error else false for each row. 
 *
 * @todo
 *  this is temporary  validation need to validate it extensive.
 * It will check row vise information
 */
function catalog_check_xls_validation_with_xls($xls_value){  
  $error = $error_info = array();
    //print_r($xls_data); die;
  //foreach($xls_data as $key => $xls_value){  
    
 
     $status = 'TRUE';
     $error_message ='';
	// To check title should not be blank
	if(empty($xls_value[1]) || trim($xls_value[1]) == ""){
	  
	   $status = 'False';  
      $error_message .= 'Title should not be blank';      	                  
	
	}
    
   	if(trim($xls_value[1]) == "-"){	  
	   $status = 'False';  
      $error_message .= 'Hypen(-) should not be in Title';      	                  
	
	}
	
    #################################### Check ISBN ######################
	if(empty($xls_value[2]) || trim($xls_value[2]) == ""){
	   $status = 'False';
	  //$error[$key]['status'] = 'FALSE';	  
      $error_message .= isset($error_message) ? '| ISBN3 should not be blank' : 'ISBN3 should not be blank'; 	 
	}else{	   
      $length_isbn = strlen($xls_value[2]);       
      if($length_isbn > 20){                  
        $status = 'False';
	    $error_message .= isset($error_message) ? '| ISBN3 value is length is more than 20 Char' : 'ISBN3 value is length is more than 20 Char';                               
      }
	}  
   #################################### Check ISBN ######################
   
	//empty($xls_value[1]) || trim($xls_value[1]) == ""
	if(empty($xls_value[18]) || trim($xls_value[18]) == ""){
       $status = 'False';
	   // To check category genre should not be blank	
	   $error_message .= isset($error_message) ?' | category genre should not be blank' : ' category genre should not be blank'; 	                        
	}
	
	if(empty($xls_value[19]) || trim($xls_value[19]) == ""){	
	     $status = 'False';
	    // To check ebay category id value should not be blank	    
        $error_message .= isset($error_message) ? '| ebay category id should not be blank' : 'ebay category id should not be blank'; 	                          
	   //return false;
	}
	
	if(empty($xls_value[21])|| trim($xls_value[21]) == ""){	
	     // To check MRP value should not be blank
	    $error_message .= isset($error_message) ? '| MRP value should not be blank' : ' MRP value should not be blank'; 
        $status = 'False';
						    
	  //return false;
	}
    
    
    //Seller id
   	if(empty($xls_value[26])|| trim($xls_value[26]) == ""){		    
        $error_message .= isset($error_message) ? ' | Seller id value should not be blank' : 'Seller id value should not be blank';
  	    $status = 'False';                     
	  //return false;
	}
    
    $error_info = array(
                    'error_message' => $error_message,
                    'status' => $status
                );    
    
  return $error_info;
}
