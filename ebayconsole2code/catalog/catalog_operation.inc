<?php

/**
 * Delete catalog for multiple ISBN
 */
function catalog_delete_catalog_op($form,&$form_state){
    $form = array();
    $form['isbn_fieldset'] = array(
      '#type' => 'fieldset',		
	    '#title' => t('Delete catalog by ISBN.'),
    );   
   
	
   $form['isbn_fieldset']['isbn_filter_option_val']= array(
      '#type' => 'textarea',  
      '#prefix' => '<div class="isbn-filter-option">',
      '#suffix' => '</div>',
      '#title' => t('ISBN values'),	  
      '#description' => '#Note : Enter each ISBN value line by line, Please do not keep blank line in text area.',	  
   );
    
   $form['isbn_fieldset']['delete_cat_isbns'] = array(
    '#type' => 'submit',
    '#value' => t('Delete'),
   );
   
   return $form;
}

/**
 * catalog filter form validation
 * 
 */
function catalog_delete_catalog_op_validate($form, &$form_state){  
 //  print_r($form_state['values']['isbn_filter_option_val']); die;
   $isbn_filter_option_val = $form_state['values']['isbn_filter_option_val'];
 // print_r($isbn_filter_option_val); die;
   if($isbn_filter_option_val){
       $exlpolded_isbn =  explode("\n",$isbn_filter_option_val);
       // print_r($exlpolded_isbn); die;
       foreach($exlpolded_isbn as $exlpolded_isbn_vals){
        if(!is_numeric(trim($exlpolded_isbn_vals))){
             form_set_error('isbn_filter_option_val', 'Inputed value : '. "<b>".$exlpolded_isbn_vals."</b>" .'  in Filter by ISBN is not a numeric.');
        }        
       }    
   }
}


/**
 * delete catalog submit handler  120038366 102702586 	120038366
 */
function catalog_delete_catalog_op_submit($form,&$form_state){
   $isbn_filter_option_val = $form_state['values']['isbn_filter_option_val'];
    //echo "****";
   //print_r($isbn_filter_option_val);die;
   if($isbn_filter_option_val){
       $exlpolded_isbn =  explode("\n",$isbn_filter_option_val);
       
        foreach($exlpolded_isbn as $value){
            // trim the ASCII control characters at the beginning and end of $binary
            // (from 0 to 31 inclusive)
            $isbn =  trim(trim($value,"\x00..\x1F"));
            
            $isbn_nm = catalog_get_nid_by_isbn($isbn);
            if($isbn_nm){
                 $array_ids[] = $isbn_nm;
            }else{
                 $array_ids_wrong[] = $isbn;
                 $error = 'These are '.implode(',',$array_ids_wrong) .' wrong ISBN that can not be deleted.';                 
            }
           
        }
        //print_r($exlpolded_isbn); die;
        $table_name = 'catalog';
        $field_name = 'isbn13';     
        $mes = 0; 
        
        if(is_array($array_ids)){
            ebay_books_delete_records_mulitiple($table_name,$field_name,$array_ids);  
            
            $mes = 'All '.implode(',',$array_ids).' ISBN records removed successfully!';
        }
        
        $message = $mes ;      
        if($message){
            drupal_set_message($message);
        }
        
        if($error){
          drupal_set_message($error,'error');   
        }
   }
}

/**
 * Update catalog by certain column
 */
function catalog_update_by_column_op($form,&$form_state){
    $file = $form_state['values']['file'];
    $file_name = $file->filename; 
    $file_cat_ref = drupal_get_path('module', 'catalog') . "/template_upldate_catalog.xlsx";//
    $form['catalog_upload'] = array(
		'#type' => 'file',
		'#title' => t('Choose a file'),
		'#title_display' => 'invisible',
		'#size' => 22,      
		'#description' => $file_name .'#Note : You can update one column at a time.'."<br>". '<h3>To download catalog update template file click ' .l('Here', $file_cat_ref).'</h3>',
		'#weight' => -10,
	);
	
    $form['read_excel_file'] = array(
		'#type' => 'submit', 
		'#value' => t('Upload'),
	);
    
  return $form;
}

function catalog_update_by_column_op_validate($form,&$form_state){
    //require_once 'Classes/PHPExcel.php';
    ############################
    require_once 'simple/simplexlsx.class.php'; //This is for xlsx reader
    require_once 'excel_reader2.php';//This is for xls reader
   
	$file = file_save_upload('catalog_upload', array('file_validate_extensions' => array('xls xlsx'),));
	
 //	dpr($file); die;
    
	if ($file) {
      if ($file = file_move($file, 'public://')) {
        $form_state['values']['file'] = $file;
      }else {
        form_set_error('file', t('Failed to write the uploaded file the site\'s file folder.'));
      }
    }
	

	$file = $form_state['values']['file'];
	$file->status = FILE_STATUS_PERMANENT;
	$filepath = drupal_realpath($file->uri);
    
    ###################mapping of excel readers###################
     $ext = pathinfo($filepath, PATHINFO_EXTENSION);
    ##############################################################
        
     $header = array('title'=>'Title', 'isbn13'=>'ISBN13', 'isbn' => 'ISBN','author1'=>'Author 1','author2'=>'Author 2',
       'author3'=>'Author 3','author4'=>'Author 4', 'author5'=>'Author 5', 'author6'=>'Author 6','author7'=>'Author 7','author8'=>'Author 8','author9'=>'Author 9','author10'=>'Author 10',
  	   'author_introduction' =>'Author Introduction','book_description' => 'Book description', 'book_image'=>'Book Image','book_reviews'=> 'Book reviews','category_genre' => 'Category/ genre', 
       'ebay_category_id_1' => 'eBay category ID 1',
       'ebay_category_id_2' => 'eBay category ID 2', 'mrp_inr'=>'MRP (INR)',
       'number_of_pages'=>'Number of pages','published_date_edition'=>'Published date/ Edition', 
       'publisher' =>'Publisher', 'type_hardcover_paperback' => 'Type - Hardcover/ Paperback',
       'seller_id' => 'Seller Id','edition'=> 'Edition','rating' => 'Rating',
       'meta_keywords' => 'Meta Keywords','meta_description' => 'Meta  Description');
  
    
     if($ext == 'xls'){
          
          $data = new Spreadsheet_Excel_Reader($filepath);
          $numrows = count($data->sheets[0]['cells']);
          $numcols = $data->sheets[0]['numCols'];  
          
          //echo $numcols; die;
          if($numcols != 2){
    	    form_set_error('catalog_upload', 'Uploaded file doesn\'t match column count.');
    	  } 
    	
          if($numcols > 500000){
              form_set_error('catalog_upload', 'Uploaded file has more than fifty thousand records. Please upload it max fifty thousand records only.');
          }
          
          
        $row = 1;   
        $col = 2;   //COLUMN START FROM 1 
        // print_r($output[$row]);die;
        $output[$row][$col] = $data->val($row, $col, $sheet = 0);// It will read data from xls
        $coulm_to_search = $output[$row][$col]; 
        
            
        if($column_name_to_update = array_search($coulm_to_search,$header)){
        
        }else{
            form_set_error('catalog_upload','Uploaded file column name is invalid, please recheck.');
        }
        
     }else if($ext == 'xlsx'){
         //echo "***";die;
         //form_set_error('Uploaded file extension is invalid, please recheck.','error');
         
          $data = new SimpleXLSX($filepath);
          $test =  list($numcols, $numrows) = $data->dimension();
         
         
          $info = $data->rows();
          
           //echo $numcols; die;
          if($test[0] != 2){
    	    form_set_error('catalog_upload', 'Uploaded file doesn\'t match column count.');
    	  } 
    	
          if($test[1] > 300000){
              form_set_error('catalog_upload', 'Uploaded file has more than fifty thousand records. Please upload it max fifty thousand records only.');
          }
           
          $coulm_to_search = $info[0][1]; 
      
         if($column_name_to_update = array_search($coulm_to_search,$header)){
             // drupal_set_message('Uploaded file column name is valid, please recheck.','error');echo "***"; die;
         }else{
           form_set_error('catalog_upload','Uploaded file column name is invalid, please recheck.'); //echo "(*(*("; die;
         }
       //echo "&&&&&"; die;
     }//End of XLSX if loop	     
   
}


function catalog_update_by_column_op_submit($form,&$form_state){   
    ############################
    require_once 'simple/simplexlsx.class.php'; //This is for xlsx reader
    require_once 'excel_reader2.php';//This is for xls reader
    
    $file = $form_state['values']['file'];
    $file_name = $file->filename; 
	$file->status = FILE_STATUS_PERMANENT;
	$filepath = drupal_realpath($file->uri);       

   
    ###################mapping of excel readers###################
     $ext = pathinfo($filepath, PATHINFO_EXTENSION);
    ##############################################################
      
      //echo $ext ; die;
        
     $header = array('title'=>'Title', 'isbn13'=>'ISBN13', 'isbn' => 'ISBN','author1'=>'Author 1','author2'=>'Author 2',
       'author3'=>'Author 3','author4'=>'Author 4', 'author5'=>'Author 5', 'author6'=>'Author 6','author7'=>'Author 7','author8'=>'Author 8','author9'=>'Author 9','author10'=>'Author 10',
  	   'author_introduction' =>'Author Introduction','book_description' => 'Book description', 'book_image'=>'Book Image','book_reviews'=> 'Book reviews','category_genre' => 'Category/ genre', 
       'ebay_category_id_1' => 'eBay category ID 1',
       'ebay_category_id_2' => 'eBay category ID 2', 'mrp_inr'=>'MRP (INR)',
       'number_of_pages'=>'Number of pages','published_date_edition'=>'Published date/ Edition', 
       'publisher' =>'Publisher', 'type_hardcover_paperback' => 'Type - Hardcover/ Paperback',
       'seller_id' => 'Seller Id','edition'=> 'Edition','rating' => 'Rating',
       'meta_keywords' => 'Meta Keywords','meta_description' => 'Meta  Description');

  
   
     if($ext == 'xls'){  
          $data = new Spreadsheet_Excel_Reader($filepath);
          $numrows = count($data->sheets[0]['cells']);
          $numcols = $data->sheets[0]['numCols'];  
            //xls start from  1
          for($r = 1; $r <=$numrows; $r++){
            $row = $r;  
            for ($col = 1; $col <= $numcols; $col++) {
			      $output[$row][$col] = $data->val($row, $col, $sheet = 0);            
            }
              //print_r($output); die;
            
            if($row == 1){
             $coulm_to_search = $output[1][2]; 
             // print_r($coulm_to_search); die;
            }else{
        
               if($column_name_to_update = array_search($coulm_to_search,$header)){
                  // echo $filed_name; die;
                  //Write here code to update field
                  $table = 'catalog';   
                 //    echo $row; 
                  $isbn_column_value = $output[$row][1];//ISBN value
                  $column_value = $output[$row][2]; // Field value to update.
                  
                  $isbn_column_name = 'isbn13'; //DB column name
                  
                  $update_values = array($column_name_to_update => $column_value);
                  
                  if($coulm_to_search == 'MRP (INR)'){  
                    $mrp_inr = $column_value ; $pub = NULL;
                    
                    // Calculating listing price on MRP update.
                    $listing_price = stock_price_calculator($isbn13,floatval($mrp_inr),$pub);
                    $updatel_isting_price  = array('listing_price'=> $listing_price);
                    
                    $update_values = array_merge($update_values, $updatel_isting_price);
                  }
                 // print_r($update_values); die;
                  ebay_books_update_records($table,$isbn_column_name,$isbn_column_value,$update_values);            
               }
             }//End inner for loop 	
         }  //End outer for loop 	
                        
       //drupal_set_message('Catalaog updated successfully.');             
          
     }else if($ext == 'xlsx'){  
         
          $data = new SimpleXLSX($filepath);
          $test =  list($numcols, $numrows) = $data->dimension();
          
          $numrows  = $test[1];
          $ColumnCount = $test[0];
          
          $info = $data->rows();          
          //xlsx start from zero 0
          for($r = 0; $r < $numrows; $r++){
            $row = $r;  
            foreach($info[$r] as $col => $cols){
             $output[$row][$col+1] = $cols;        
            }  
            
            if($row == 0){
             $coulm_to_search = $output[0][2]; 
             // print_r($coulm_to_search); die;
            }else{
        
               if($column_name_to_update = array_search($coulm_to_search,$header)){
                  // echo $filed_name; die;
                  //Write here code to update field
                  $table = 'catalog';   
                 //    echo $row; 
                  $isbn_column_value = $output[$row][1];//ISBN value
                  $column_value = $output[$row][2]; // Field value to update.
                  
                  $isbn_column_name = 'isbn13'; //DB column name
                  
                  $update_values = array($column_name_to_update => $column_value);
                  
                  if($coulm_to_search == 'MRP (INR)'){  
                    $mrp_inr = $column_value ; $pub = NULL;
                    
                    // Calculating listing price on MRP update.
	$listing_price = stock_price_calculator($discount,$isbn13,$qty,$nid);
      
                    $updatel_isting_price  = array('listing_price'=> $listing_price);
                    
                    $update_values = array_merge($update_values, $updatel_isting_price);
                  }
                  //print_r($isbn_column_value); die;
                  ebay_books_update_records($table,$isbn_column_name,$isbn_column_value,$update_values);            
               }
             }//End inner for loop 	
         }              
      
    }//End of XLSX if loop	
     drupal_set_message($file_name. ' File updated successfully.');     
    return true;     

}


