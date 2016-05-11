<?php

/**
 * Implements callback of discount/list_all/promotional_books_expiry
 */
function discount_list_all_promotional_books_expiry(){
   $exp_days = 2; //No of days after promo book will expire
   $options =  discount_find_promo_book_expiring($exp_days);
    
   // dpr($opti);
    //die;
    $header = array();
    //$options = array();
    $default_value = array();
    $IsCheckbox = true;
    
    $header = array('promo_name' => 'Promo Name','promo_isbn' => 'ISBN Number','promo_days' => 'Promo Days','promo_discount' => 'Promo Discount','expires' => 'Promo Expires On',);
   
    if($IsCheckbox){
        $default_value[0] = true;
        $default_value[1] = false;
    }else{
     $default_value = '0';
    }
  
 
        
        
    $form['renew_delete_selector'] = array (
        '#type' => 'fieldset',
        '#title' => 'Renew Or Delete',
        '#header' => t('Renewel Form'),    
        //'#attributes' => array(
                        // 'class' => array('collapsible', 'collapsed'),
        );
    
    
     
     $form['renew_delete_selector']['operation']  =  array (
        '#type' => 'select',
        '#title' => 'Operation',
        '#options' => array(''=>'Select','renew'=>'Renew','delete'=>'Delete'),    
       
     );  
   
     $form['renew_delete_selector']['Period']  =  array (
        '#type' => 'textfield',
        '#title' => 'Days',
       
       
     );  
      
       $form['renew_delete_selector']['Apply']  =  array (
        '#type' => 'submit',       
        '#value' => 'Apply' 
     );  
   
    
    
    
    $form['myselector'] = array (
        '#type' => 'tableselect',
        '#title' => 'My Selector',
        '#header' => $header,
        '#options' => $options,
        '#multiple' => $IsCheckbox,
        '#default_value' => $default_value
    );
   
   if(!$options){
     $form['no_rec_promo'] = array ( 
         '#markup' => "No records found",
     );
   } 
   
    $form['page_shown'] = array (        
        '#markup' => theme("pager"),    
    );   
   
 return $form;
}

/**
 * Promotional book submit handler
 * 
 */
function discount_list_all_promotional_books_expiry_submit($form,&$form_state){
    //dpr($form_state); die;
    $myselector_values = $form_state['values']['myselector'];
    $operation =  $form_state['values']['operation'];
    
     $selected_count=0;        
        foreach($myselector_values as $prm_id => $sel_value){
          if($sel_value){
             $renew_select_promo[] = $sel_value;
             $selected_count++;
          }
        }//End for loop
/*
*Validations
*
*/
$error_check=0;
  if($form_state['values']['Period']=='' &&( $operation=='renew' )){
        form_set_error('Period',t('Select no of days'));
        $error_check=1;
    }

   if($operation!='renew' && $operation!='delete'){
        form_set_error('operation',t('Select an operation'));
         $error_check=1;
    }
   if($selected_count==0){
        form_set_error('myselector',t('No records selected for operation'));
         $error_check=1;
    
  }


 if($error_check==0)
 {

    
    
    if($myselector_values){
        $selected_count=0;        
        foreach($myselector_values as $prm_id => $sel_value){
          if($sel_value){
             $renew_select_promo[] = $sel_value;
             $selected_count++;
          }
        }//End for loop
    

     
    
  
    
    // dpr($renew_select_promo); die;
    if($operation == 'renew'){
        $table_name = 'ebay_books_promotions';     
         $promo_days = $form_state['values']['Period'];
       
        $update_values = array(
                         'promo_time_from'=>time(),
                         'promo_days'=> $promo_days,
                         //'promo_discount'=> $promo_discount,
                         );
       
        $key_fields = 'promo_id';
        $key_field_value = ''; 
       
      $query =  db_update($table_name);
       $query->fields($update_values);
       $query->condition($key_fields, $renew_select_promo, 'in');  
      //   dpq($query);     
       $query->execute();
      drupal_set_message($selected_count.' Promotional Books renewed.');
      
    
    }
    
    
    if($operation == 'delete'){
        $table_name = 'ebay_books_promotions';  
         $key_fields = 'promo_id';   
         $query =  db_delete($table_name);
         $query->condition($key_fields, $renew_select_promo, 'in');  
         $query->execute();
        
          drupal_set_message('Record Deleted Successfully');
        //echo "Delete";die;
    }
     
//dpr($myselector_values);die;



   
}
}
  
    return TRUE;
}
