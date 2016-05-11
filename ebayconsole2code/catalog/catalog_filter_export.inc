<?php

/**
 * Include css files as necessary.
 *
 * This helper function is used by ctools but can also be used in other
 * modules in the same way as explained in the comments of ctools_include.
 *
 * @param $file
 *   The base file name to be included.
 * @param $module
 *   Optional module containing the include.
 * @param $dir
 *   Optional subdirectory containing the include file.
 */
function catalog_filter_add_css($file, $module = 'catalog', $dir = 'css') {
  drupal_add_css(drupal_get_path('module', $module) . "/$dir/$file.css");
}
    

/**
 * This example shows a button to "add more" - add another textfield, and
 * the corresponding "remove" button.
 *
 * It works equivalently with javascript or not, and does the same basic steps
 * either way.
 *
 * The basic idea is that we build the form based on the setting of
 * $form_state['num_names']. The custom submit functions for the "add-one"
 * and "remove-one" buttons increment and decrement $form_state['num_names']
 * and then force a rebuild of the form.
 *
 * The $no_js_use argument is simply for demonstration: When set, it prevents
 * '#ajax' from being set, thus making the example behave as if javascript
 * were disabled in the browser.
 */
function catalog_filter_add_more($form, &$form_state, $no_js_use = FALSE) {
  // Add css file 
  catalog_filter_add_css('catalog');    
  // print_r($form_state);
  /*$form['description'] = array(
    '#markup' => '<div>' . t('This example shows an add-more and a remove-last button. The <a href="!ajax">AJAX version</a> does it without page reloads; the <a href="!multistep">non-js version</a> is the same code but simulates a non-javascript environment, showing it with page reloads.',
      array('!ajax' => url('examples/catalog_filter/add_more'), '!multistep' => url('examples/catalog_filter/add_more_no_js')))
    . '</div>',
  );*/

  // Because we have many fields with the same values, we have to set
  // #tree to be able to access them.
  
  $disabled = 0;
  //$_SESSION['show_download'] = 0;   
  $form['#tree'] = TRUE;
  $form['names_fieldset'] = array(
    '#type' => 'fieldset',
    '#title' => t('Filter a catalog to download XLS file.'),
    // Set up the wrapper so that AJAX will be able to replace the fieldset.
    '#prefix' => '<div id="names-fieldset-wrapper">',
    '#suffix' => '</div>',
  );

  // Build the fieldset with the proper number of names. We'll use
  // $form_state['num_names'] to determine the number of textfields to build.
  if (empty($form_state['num_names'])) {
    $form_state['num_names'] = 1;
  }
  
  //Select list options 
  /*$option_list = array('Title','Author 1','Author 2','Author 3','Author 4','Author 5','Author 6','Author 7','Author 8','Author 9','Author 10',
                    'Author Introduction','Book description','Book Image','Book reviews','Category/ genre','eBay category ID 1','eBay category ID 2','MRP (INR)','Number of pages','Published date/ Edition',
                   'Publisher','Type - Hardcover/ Paperback','Seller Id','Edition','Rating','Meta Keywords','Meta  Description');
  */
  
  
  $option_list = array('Title','Author 1','Category/ genre','eBay category ID 1','Publisher','Seller Id');

  
  $form['isbn_fieldset'] = array(
      '#type' => 'fieldset',		
	  '#title' => t('Filter by ISBN.'),
    );
    
  $form['isbn_fieldset']['fiter_isbn_options'] = array(
      '#type' => 'select',
	  '#options' => array('ISBN13' => 'ISBN13'),
      '#title' => t('Select Filter : '),	
      '#prefix' => '<div class="select-filter-options">',
      '#suffix' => '</div>',	  
    );
	
   $form['isbn_fieldset']['isbn_filter_option_val']= array(
      '#type' => 'textarea',  
      '#prefix' => '<div class="isbn-filter-option">',
      '#suffix' => '</div>',
      '#title' => t('ISBN values'),	  
      '#description' => '#Note : Enter each ISBN value line by line.',	  
    );
  
  $options = array_combine($option_list,  $option_list);
 
  $n = 0;
  
  //Start for loop
  for($i = 0; $i < $form_state['num_names']; $i++) {
    $n++;
	$form['names_fieldset']['filter'][$i] = array(
		'#type' => 'fieldset',		
		'#prefix' => '<div class="filter-options-fieldset">',
		'#suffix' => '</div>',
    );
    
	$form['names_fieldset']['filter'][$i]['option_selected'][$i] = array(
      '#type' => 'select',
	  '#options' => $options,
      '#title' => t('Select Filter : '),	
      '#prefix' => '<div class="select-filter-options">',
      '#suffix' => '</div>',	  
    );
	
   $form['names_fieldset']['filter'][$i]['input_value'][$i] = array(
      '#type' => 'textfield',
	  '#size' => 50,
      '#title' => t('Value : '),	
      '#prefix' => '<div class="filter-value">',
      '#suffix' => '</div>',	  
    );
  }//End for loop of Add more form

  
  
  $form['names_fieldset']['add_name'] = array(
    '#type' => 'submit',
    '#value' => t('Add one more'),
    '#submit' => array('catalog_filter_add_more_add_one'),
    // See the examples in catalog_filter.module for more details on the
    // properties of #ajax.
    '#ajax' => array(
      'callback' => 'catalog_filter_add_more_callback',
      'wrapper' => 'names-fieldset-wrapper',
    ),
  );
  
  if ($form_state['num_names'] > 1) {
    $form['names_fieldset']['remove_name'] = array(
      '#type' => 'submit',
      '#value' => t('Remove one'),
      '#submit' => array('catalog_filter_add_more_remove_one'),
      '#ajax' => array(
        'callback' => 'catalog_filter_add_more_callback',
        'wrapper' => 'names-fieldset-wrapper',
      ),
    );
  }
  
  $form['date_check'] = array(
      '#type' => 'checkbox',
      '#title' => t('Filter by Date'),	 
	  '#default_value' => 0,     
  );
       
    $form['date_events_op'] = array(
      '#type' => 'radios',
      '#title' => t('Date By'),	 
      '#options' => array('created'=> 'Created','updated'=>'Updated'),  
      '#default_value' => 'created', 
      '#prefix' => '<div id="date-event-fieldset-wrapper">',
      '#suffix' => '</div>',  
    ); 
  
  
  
  $form['from_date'] = array(
    '#type' => 'date_popup',
    '#title' => 'From Date',
	'#default_value' => date('Y-m-d 00:00:00'),
    '#date_type' => DATE_DATETIME,  
    '#date_format' => 'm/d/Y - H:i',  
    '#date_year_range' => '-3:+3',
    '#prefix' => '<div class="filter-from-date">',
    '#suffix' => '</div>',
  );
 
  $form['to_date'] = array(
    '#type' => 'date_popup',  
    '#title' => 'To Date',
	'#default_value' => date('Y-m-d 00:00:00'),
    '#date_type' => DATE_DATETIME,   
    '#date_format' => 'm/d/Y - H:i',  
    '#date_year_range' => '-3:+3',
    '#prefix' => '<div class="filter-to-date">',
    '#suffix' => '</div>',
  );
  	    
    $form['Show_count'] = array(
    '#type' => 'submit',
    '#value' => t('Show Count'),
	'#ajax' => array(
		  'callback' => 'ajax_example_submit_driven_callback',
		  'wrapper' => 'show-count-wrapper',
        ),
    );
	
   
	$form['download'] = array(
	 '#type' => 'submit',
	 '#value' => t('Download'),	
	 '#submit' => array('catalog_filter_download_xls'), 
    ); 
	
	
	$form['box'] = array(
    '#type' => 'markup',
    '#prefix' => '<div id="show-count-wrapper">',
    '#suffix' => '</div>',
    '#markup' => '<h1> Caution: More than ten lacks records can not be downlaoad.</h1>',
    );
  

  // This simply allows us to demonstrate no-javascript use without
  // actually turning off javascript in the browser. Removing the #ajax
  // element turns off AJAX behaviors on that element and as a result
  // ajax.js doesn't get loaded.
  // For demonstration only! You don't need this.
  if ($no_js_use) {
    // Remove the #ajax from the above, so ajax.js won't be loaded.
    if (!empty($form['names_fieldset']['remove_name']['#ajax'])) {
      unset($form['names_fieldset']['remove_name']['#ajax']);
    }
    unset($form['names_fieldset']['add_name']['#ajax']);
  }

  return $form;
}


/**
 * Select the 'box' element, change the markup in it, and return it as a
 * renderable array.
 *
 * @return renderable array (the box element)
 */
function ajax_example_submit_driven_callback($form, &$form_state) {   
    
  $filter_input = $form_state['values'];

  $filter_extra_info = catalog_hander_inputed_filter($filter_input);  
  
  $user_submitted_info = $form_state['values']['names_fieldset']['filter'];  
  
 
  $Show_count =  $form_state['values']['Show_count']; 
  $date_checked_value = array('date_filter_set' => $form_state['values']['date_check'], 'date_operation' => $form_state['values']['date_events_op']);
  
 
  $frm_date =  $form_state['values']['from_date']; 
  $from_time = catalog_generate_uinx_time_stamp($frm_date);  
  
  // echo date('d-m-Y h:i:s'$from_time)  ;die;
  
  $to_date =  $form_state['values']['to_date']; 
  $to_time = catalog_generate_uinx_time_stamp($to_date); 
  
  $filter_count = array();
  
  //echo $from_time ."to date" . $to_time ."*&*&*". print_r($user_submitted_info) ."datecheck ooption".$date_checked_value;
  // Pass all imputed information as a parameter to find available catalog details.
  $filter_count  = catalog_serach_filtered_input($from_time, $to_time, $user_submitted_info,$filter_extra_info,$date_checked_value);
 
  $element = $form['box'];  
  $element['#markup'] = 'Click downlaod,  to fetch total <b>'. count($filter_count).'</b> Records.';
  return $element;
}


/**
 * Callback for both ajax-enabled buttons.
 *
 * Selects and returns the fieldset with the names in it.
 */
function catalog_filter_add_more_callback($form, $form_state) {
  return $form['names_fieldset'];
}

/**
 * Submit handler for the "add-one-more" button.
 *
 * Increments the max counter and causes a rebuild.
 */
function catalog_filter_add_more_add_one($form, &$form_state) {
  $form_state['num_names']++;
  $form_state['rebuild'] = TRUE;
}

/**
 * Submit handler for the "remove one" button.
 *
 * Decrements the max counter and causes a form rebuild.
 */
function catalog_filter_add_more_remove_one($form, &$form_state) {
  if ($form_state['num_names'] > 1) {
    $form_state['num_names']--;
  }
  $form_state['rebuild'] = TRUE;
}

/**
 * catalog filter form validation
 * 
 */
function catalog_filter_add_more_validate($form, &$form_state){  
   $fiter_isbn_options = $form_state['values']['isbn_fieldset']['fiter_isbn_options'];
   $isbn_filter_option_val = $form_state['values']['isbn_fieldset']['isbn_filter_option_val'];
  
  // if($isbn_filter_option_val){
       $exlpolded_isbn =  explode("\n",$isbn_filter_option_val);// print_r($exlpolded_isbn); die;
       //foreach($exlpolded_isbn as $exlpolded_isbn_vals){
//        if(!is_numeric(trim($exlpolded_isbn_vals))){
//             form_set_error('isbn_filter_option_val', 'Inputed value : '. "<b>".$exlpolded_isbn_vals."</b>" .'  in Filter by ISBN is not a numeric.');
//        }        
//       }    
  // }
   //print_r($isbn_filter_option_val); die;
  //$filter_info =  $form_state['values']['names_fieldset']['filter']; 
  
  
  $from_date =  $form_state['values']['from_date']; 
  $from_unx_time = catalog_generate_uinx_time_stamp($from_date);  
  
  $to_date =  $form_state['values']['to_date']; 
  $to_unx_time = catalog_generate_uinx_time_stamp($to_date);
  
  if($from_unx_time > $to_unx_time){
     form_set_error('from_date', 'From date sholud be lesser than To date.');
  }
}

/**
 * Handel's filter input and arrange in a required format.
 * 
 */
function catalog_hander_inputed_filter($filter_input){
   $filter_xtra_info = array();
  //Need to test it  
  ################################## Handeling ISBN FILTER ###############
    $fiter_isbn_options = $filter_input['isbn_fieldset']['fiter_isbn_options'];
    $isbn_filter_option_val = $filter_input['isbn_fieldset']['isbn_filter_option_val'];
    //$exlpolded_isbn =  explode("\n",$isbn_filter_option_val);  names_fieldset
  
   ################################### END OF Handeling ISBN FILTER ###############
   
   //Creating new array option for ISBN filter inputed value
   $filter_xtra_info[$fiter_isbn_options] = $isbn_filter_option_val;
   
   return $filter_xtra_info;
}


/**
 * Final submit handler.
 *
 * Reports what values were finally set.
 */
function catalog_filter_download_xls($form, &$form_state) {
  $inputed_filter = '';
  $catalog_nids = array();
  $filter_input = $form_state['values'];
  $user_submitted_info = $form_state['values']['names_fieldset']['filter'];  
  $filter_extra_info = catalog_hander_inputed_filter($filter_input);  
  //print_r($filter_extra_info); die;
  //Create unix timestamp from date/time  
  $from_date =  $form_state['values']['from_date']; 
  $from_unx_time = catalog_generate_uinx_time_stamp($from_date);  
  
  //Create unix timestamp from date/time 
  $to_date =  $form_state['values']['to_date']; 
  $to_unx_time = catalog_generate_uinx_time_stamp($to_date);  
  
  $query = '';
 // $date_check = $form_state['values']['date_check'];
  $date_checked = array('date_filter_set' => $form_state['values']['date_check'], 'date_operation' => $form_state['values']['date_events_op']);
  
  
  
  // Pass all imputed information as a parameter to find available catalog details.
  $catalog_nids = catalog_serach_filtered_input($from_unx_time, $to_unx_time, $user_submitted_info,$filter_extra_info, $date_checked);
  //print_r($catalog_nids);die;
  
  $header = array('Title','ISBN13','ISBN','Author 1','Author 2','Author 3','Author 4','Author 5','Author 6','Author 7','Author 8','Author 9','Author 10',
				'Author Introduction','Book Description','Book Image','Book Reviews','Category/genre','eBay Category Id 1','eBay Category Id 2','MRP(INR)','Number of pages','Published Date/Edition',
			   'Publisher','Type - Hardcover/ Paperback','Seller Id','Edition','Language','Rating','Meta Keywords','Meta Description');
  
  
  //if(count($catalog_nids) > 0){
  foreach($catalog_nids as $nid_ky => $filter_val){
	// Write a code here to generate XLS file with catalog details.  
  
	  //Create/Download XLS here
    $rows = array();
    $rows[]= $filter_val->title;   
    $rows[]=$filter_val->isbn13; 
    $rows[]=$filter_val->isbn;   
      
	  $rows[]= $filter_val->author1;   
	  $rows[]= $filter_val->author2;   
	  $rows[]= $filter_val->author3; 
    $rows[]= $filter_val->author4;   
	  $rows[]= $filter_val->author5;
	  
	  $rows[]= $filter_val->author6;   
	  $rows[]= $filter_val->author7; 
    $rows[]= $filter_val->author8;   
	  $rows[]= $filter_val->author9;	  
	  $rows[]= $filter_val->author10;
         
	  $rows[]= $filter_val->author_introduction; 
    $rows[]= trim($filter_val->book_description);   
	  $rows[]= $filter_val->book_image;// Need to work on this keep validation to have .jpg extension by default while upload
	  
	  $rows[]= $filter_val->book_reviews;   
	  $rows[]= $filter_val->category_genre; 
    $rows[]= $filter_val->ebay_category_id_1;   
	  $rows[]= $filter_val->ebay_category_id_2;
	  
	  $rows[]= $filter_val->mrp_inr;   
	  $rows[]= $filter_val->number_of_pages; 
    $rows[]= $filter_val->published_date_edition;   
	  $rows[]= $filter_val->publisher;
	  
	  $rows[]= $filter_val->type_hardcover_paperback;   
	  $rows[]= $filter_val->seller_id; 
    $rows[]= $filter_val->edition;   
     $rows[]= $filter_val->language;  
	  $rows[]= $filter_val->rating;
	  
	  $rows[]= $filter_val->meta_keywords;   
	  $rows[]= $filter_val->meta_description; 
  //  $rows[]= $filter_val->imprint; 
   
    array_walk($rows, 'catalog_filter_clean_data');
	  $xls_content_row .= implode("\t", array_values($rows)) . "\r\n";
  }
  
  //print_r($rows);
//}else{
  //echo "No recorsfound";
//}
	$xls_content_header = implode("\t", array_values($header));
  
	$xls_content = $xls_content_header."\n".$xls_content_row;
	$filename = 'Catalog_filter_list_'.date("d_m_Y"); 
    
    
  header("Content-type: text/plain; charset=UTF-8");
	header("Content-Disposition: attachment; filename=$filename");
	header("Content-Type: application/vnd.ms-excel"); 
	header("Pragma: no-cache");
	header("Expires: 0");
	print $xls_content;
	exit();   
   return TRUE;
}



