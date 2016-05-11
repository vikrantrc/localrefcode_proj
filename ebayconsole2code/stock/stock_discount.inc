<?php


/**
 * Implements callback of stock/manage/discount
 */
function stock_manage_discount($form,&$form_state){
  drupal_add_js(drupal_get_path('module', 'stock') . '/stock.js');
  
  $options= stock_get_all_distributors();//array('12321'=> 'klkjkl');//
  
  //dpr($options); die;
  
  $default_distri = '';
  
  if(isset($_POST['distributor_name'])&& !empty($_POST['distributor_name'])){
    $default_distri = $_POST['distributor_name'];
    $publishers = stock_get_distinct_publishers($default_distri);
  }
  
  
 ############## Strat Step One ################# 
  $form['#attributes'] = array('enctype' => 'multipart/form-data');
  $form['distributor_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Distributors.'),
    '#description' => 'Step 1 : <br/><br/>Select the distributor to download discount format.',
    // Set up the wrapper so that AJAX will be able to replace the fieldset.
    //'#prefix' => '<div id="names-fieldset-wrapper">',
    //'#suffix' => '</div>',
  );
  
  //$form['#method'] = 'GET';
  $form['distributor_fieldset']['distributor_name'] = array(
  	'#type' => 'select',
  	'#title' => t('Select Distributor'), 
    '#default_value' => $default_distri,
  	'#required' => TRUE,
  	'#options' => $options,
    '#tree' => TRUE,
  );
  
  
  
  $active = array(
    'DLD'  => 'Distributor Level discount',
    'DPD'  => 'Distributor Publisher Level discount',
    'DPCD' => 'Distributor Publisher Catagory Level discount',
    'PLD'  => 'Publisher Level Discount',    
  );
  
  $form['distributor_fieldset']['discount_type'] = array(
    '#type' => 'radios',
    '#title' => t('Distributor Type'),
    '#default_value' => 'DLD',
    '#options' => $active,
    // '#description' => t('When a poll is closed, visitors can no longer vote for it.'),
    // '#access' => $admin,
     '#ajax' => array(
      'callback' => 'stock_distributor_type_callback',
      'wrapper' =>  'stock_distributor_input_type',
    ),
  );
  
  $form['distributor_fieldset']['download_template'] = array(
    '#type' => 'submit',
    '#value' => t('Download Discount Sheet'),
  );
  ############## End of step One ################# 
  
  ############### Start of Step 2 ###################
  $form['stock_discount_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Add Discount.'),
    '#prefix' => '<div id="stock_distributor_input_type">',
    '#suffix' => '</div>',
    '#description' => 'Step 2 : <br/><br/>After updating, Select the stock discount file of respective distributor.',
    // Set up the wrapper so that AJAX will be able to replace the fieldset.
    //'#prefix' => '<div id="stock_distributor_input_type">',
    //'#suffix' => '</div>',
  ); 

  if($form_state['values']['discount_type'] == 'DLD' || empty($form_state['values']['discount_type'])){
     $form['stock_discount_fieldset']['discount_number'] = array(
    	'#type' => 'textfield',
    	'#title' => t('Add Discount'),
    	'#size' => 22,
      );    
  }else{
   
     $form['stock_discount_fieldset']['discount_sheet'] = array(
    	'#type' => 'file',
    	'#title' => t('Select Stock Discount file'),
    	'#size' => 22,
      );    
  }
  
  $form['stock_discount_fieldset']['upload_discount_sheet'] = array(
    '#type' => 'submit',
    '#value' => t('Save Discount'),
  );  
 
  //$form['stock_discount_markup'] = array(   
//    '#title' => t('Add Discount.'),
//    '#prefix' => '<div id="stock_distributor_input_type">',
//    '#suffix' => '</div>',
//  );  
  
 
  ############### End of Step 2 ###################  
  
  return $form;  
}



/**
 * Implements hook_validate
 */
function stock_manage_discount_validate($form , &$form_state){
    //print_r($form_state['values']);die;
  //print_r($_FILES);die; 
    require_once 'excel_reader2.php';
   
     if($_FILES['files']['name']['discount_sheet']){
    	 $file = file_save_upload('discount_sheet', array('file_validate_extensions' => array('xls'),));
    
    	if ($file) {
          if ($file = file_move($file, 'public://')) {
            $form_state['values']['file'] = $file;
          }else {
            form_set_error('file', t('Failed to write the uploaded file the site\'s file folder.'));
          }  
          
        }   
     }
    
    $distributor = $form_state['values']['distributor_name'];   
    $dist_file_name = explode("_",$distributor);       
    $button_clicked = $form_state['clicked_button']['#parents'][0];     
    
    if($button_clicked =="upload_discount_sheet"){
        if($form_state['values']['discount_type'] != 'DLD' && $form_state['values']['discount_type'] != 'PLD'){
            $d_file = explode("_",$_FILES['files']['name']['discount_sheet']);
          
            if($d_file[0] != $dist_file_name[0]){           
             form_set_error('file', t('uploaded file does not matched for selected distributor.'));    
            }
        }
    }  
           
    if($file = $form_state['values']['file']){
      	$file->status = FILE_STATUS_PERMANENT;
        $filepath = drupal_realpath($file->uri);          
        $data = new Spreadsheet_Excel_Reader($filepath);                
        //It stores the uploaded filename
        $filename = $file->filename;
        $dis_fil_type = explode('_',$filename);         
        
        $dis_fil_type = explode('_',$filename);         
        
        $numrows = count($data->sheets[0]['cells']); 
        $numcols = $data->sheets[0]['numCols']; 
        
        if($form_state['values']['discount_type'] == 'DPD' && $numcols != 2){
          form_set_error('file', 'Distributor publisher discount file is not correct.');  
        }
        
        
        if($form_state['values']['discount_type'] == 'DPCD' && $numcols != 3){
          form_set_error('file', 'Distributor publisher discount file is not correct.');  
        }
   }//end of if loop  
  
    
}


/**
 * Implements hook_submit
 */
function stock_manage_discount_submit($form,&$form_state){
  require_once 'excel_reader2.php';
  
  $distributor = $form_state['values']['distributor_name'];//This is distributor id
  $publishers = stock_get_distinct_publishers($distributor);
  $button_clicked = $form_state['clicked_button']['#parents'][0]; 
   $table_name = 'ebay_books_discount';  
  ############### This is for First Section download publisher file discount. ######################
  if($button_clicked == "download_template"){
    //print_r($distributor);die;
    if($form_state['values']['discount_type'] == 'DPD'){
        $header = array('PUBLISHER','DISCOUNT IN %');
        foreach($publishers as $publisher){
        	$xls_content_row .= implode("\t", array_values((array)$publisher)) . "\r\n";
        }
        $xls_content_header = implode("\t", array_values($header));
        $filename = $distributor.'_discount_sheet'; 
        
    }else if($form_state['values']['discount_type'] == 'DPCD'){
        $header = array('PUBLISHER','EBAY CATEGORY ID','DISCOUNT IN %');
       
        //Get all categories by publisher
        $publishers = stock_get_distinct_publishers_categories($distributor);
        
        foreach($publishers as $publisher){
        	$xls_content_row .= implode("\t", array_values((array)$publisher)) . "\r\n";
        }
        $xls_content_header = implode("\t", array_values($header));
        
        $filename = $distributor.'_categories_discount_sheet'; 
    }
       $xls_content = $xls_content_header."\n".$xls_content_row;
     
       
        header("Content-type: text/plain; charset=UTF-8");
        header("Content-Disposition: attachment; filename=$filename");
        header("Content-Type: application/vnd.ms-excel"); 
        header("Content-type: application/x-msexcel; charset=utf-8");
        header("Pragma: no-cache");
        header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
        header("Expires: 0");
        print $xls_content;
        exit(); 
  }else if($button_clicked =="upload_discount_sheet"){
    ############### This is for Second Section Mapped and merge publisher file discount in DB. ######################
    //dpr($form_state['values']);die;
   
    $distrib_discount_number =  $form_state['values']['discount_number'];   
    
     
    if(isset($distrib_discount_number)){    
        //Saves Distributor level discount
        $publisher_name = isset($publisher_name)?$publisher_name:'-';
        $category_id = isset($category_id)?$category_id:'-';
                              
        $discount_info = array(
                        'distributor_id' => $distributor,
                        'publisher_name' => $publisher_name,
                        'category_id' => $category_id,
                        'discount' => $distrib_discount_number,
                        );
        ebay_books_delete_records($table_name,$field_name = 'distributor_id',$discount_info['distributor_id']);  
        ebay_books_save_discount($discount_info);
        drupal_set_message(' Distributor discount saved successfully.');
    }  
    
    //Check for uploaded file
    if($file = $form_state['values']['file']){
        
      	$file->status = FILE_STATUS_PERMANENT;
        $filepath = drupal_realpath($file->uri);          
        $data = new Spreadsheet_Excel_Reader($filepath);
        //It stores the uploaded filename
        $filename = $file->filename;
        $dis_fil_type = explode('_',$filename);        
        
        $numrows = count($data->sheets[0]['cells']); 
        $numcols = $data->sheets[0]['numCols']; 
        
        
        if($form_state['values']['discount_type'] == "PLD"){
           ebay_books_manage_discount_parse_sheet(0,$numrows,$numcols,$data);
        }else{
           //print_r($dis_fil_type);die;
          //Check for filename category xls
            if($numcols == 3){
               $category_file_flag = 1;
               //echo "&&&&"; die;
               ebay_books_manage_discount_parse_sheet($distributor,$numrows,$numcols,$data,$category_file_flag);
            }//end of check category file if loop 
            
            
             //Check for filename only publisher xls
            if($numcols == 2){
              ebay_books_manage_discount_parse_sheet($distributor,$numrows,$numcols,$data);
            }  
        }
        
     
    }//end of if loop     
    
   
     //dpr($output);die;
  }//End Elseif  
  
}

/**
 * Parse uploaded discount xls file.
 */
function ebay_books_manage_discount_parse_sheet($distributor,$numrows,$numcols,$data, $category_file_flag = NULL){
        $rn = 0;
        $table_name = 'ebay_books_discount';       
        //Delete all records before save.
        ebay_books_delete_records($table_name,$field_name = 'distributor_id',$distributor);
      	for($row = 2; $row <= $numrows; $row++) {
      		  $flag = false;  
              $row_xl = array();
      			for ($col = 1; $col <= $numcols; $col++) {
      				$output[$row][$col] = $data->val($row, $col, $sheet = 0);
      				$row_xl[] = substr($output[$row][$col],0,50);
      			}//End Inner for loop               
               
                if($category_file_flag){              
                 $publisher_name = isset($output[$row][1])?$output[$row][1] : '--';
                 $category_id = isset($output[$row][2])?$output[$row][2] : '--';
                 $discount = isset($output[$row][3])?$output[$row][3] : 0;
                 $distributor_exist = stock_check_distributor_publisher_categories($distributor, $publisher_name, $category_id); 
                 if($distributor_exist){
                    $flag = true;
                 }
                }else{
                    $publisher_name = isset($output[$row][1])?$output[$row][1] : '--';
                    $discount = isset($output[$row][2])?$output[$row][2] : 0;
                    $category_id = '--';
                    $flag = true;
                }
               //Saves data only if discount has some values
               if($discount){
                    $discount_info = array(
                            'distributor_id' => $distributor,
                            'publisher_name' => $publisher_name,
                            'category_id' => $category_id,
                            'discount' => (float)$discount,
                            );
                     
                    if($flag == true ){
                        ebay_books_save_discount($discount_info); 
                        $rn++;
                    }
                    
//echo "(((" . $category_file_flag; die;
                    //if($category_file_flag){
//                        //Check selected distributor has same publisher and categories as in uploaded xls sheet before save.
//                                               
//                        if($gg){
//                            //Saves Distributor, Publisher, category level discount
//                            ebay_books_save_discount($discount_info);
//                            $rn++;  
//                        }                     
//                    }else{                      
//                        //Saves Distributor, Publisher, category level discount
//                        ebay_books_save_discount($discount_info);  
//                            
//                    }                                  
                 }              
         }//End Outer for loop
        drupal_set_message( $rn.' Records saved successfully.');
   return true;
}

/**
 * Save distributors discount.
 */
function ebay_books_save_discount($input_array){
    $table_name = 'ebay_books_discount';    
    ebay_books_save_records($table_name,$input_array);
    return TRUE;
}


/**
 * Check for publisher exist for distributor
 
function stock_publisher_exist_for_distributor($distributor,$publisher_name){
  $query = db_select('ebay_books_stock_discount','dc');
  $query->condition('dc.distributer_id', $distributor, '=');
  $query->condition('dc.Publisher', $publisher_name, '=');
  $query->fields('dc',array('id'));  
 // dpq($query);
  $disc_reds = $query->execute()->fetchField();    
  return $disc_reds;
 }*/
 
 
function stock_distributor_type_callback($form, $form_state){
   $form_state['rebuild'] = TRUE;
  return $form['stock_discount_fieldset'];
}
