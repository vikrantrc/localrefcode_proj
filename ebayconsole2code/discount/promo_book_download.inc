<?php 


function promo_book_filter_download_feed(){
   global $user;
   // $output = drupal_get_form('redemption_order_select_form');
   $output .= drupal_render(drupal_get_form('promo_book_download_feed_filter'));
   $output .= promo_book_downlaod_filter_table_view();//Get table output format with pagination.
   return $output;
}



/**
 * Implements callback of promo_book/search
 */
function promo_book_download_feed_filter($form, &$form_state){
	drupal_add_css(drupal_get_path('module', 'discount') . "/discount.css");
    drupal_add_js(drupal_get_path('module', 'discount') . '/discount.js');
        
    if($_GET['from_date']){
        $from_date = $_GET['from_date']['date'];          
        $date = explode("-",$from_date);        
        $from_date = $date[2].'-'.$date[0].'-'.$date[1];        
        //print_r($date);die;
       // $from_date = $from_date ." ". $_GET['from_date']['time'];
    }
     
    if($_GET['to_date']){
        $to_date = $_GET['to_date']['date'];        
        $date = explode("-",$to_date);        
        $to_date = $date[2].'-'.$date[0].'-'.$date[1]; //Y-m-d
      //  $to_date = $to_date." ".$_GET['to_date']['time'];
    }    
    
    
 	$form = array();    
	$form['from'] = array(
		'#type' => 'fieldset',
		'#title' => t('From Date'),
		'#weight' => 0,
		'#collapsible' => TRUE,
		'#collapsed' => FALSE,
		'#prefix' => '<div class="promo_book-filter-from-date">',
		'#suffix' => '</div>',
	);
	
    $form['from']['from_date'] = array(
		'#type' => 'date_popup',
		'#date_type' => DATE_DATETIME,  
        '#default_value' => isset($from_date) ? $from_date : '',
		'#date_format' => 'm-d-Y - H:i',  
		'#date_year_range' => '-3:+3',
	);
	
    $form['to'] = array(
		'#type' => 'fieldset',
		'#title' => t('To Date'),
		'#weight' => 1,
		'#collapsible' => TRUE,
		'#collapsed' => FALSE,
		'#prefix' => '<div class="promo_book-filter-to-date">',
		'#suffix' => '</div>',
	);
    
    //echo $to_date;
    
	$form['to']['to_date'] = array(
		'#type' => 'date_popup',  
		'#date_type' => DATE_DATETIME,   
        '#default_value' => isset($to_date) ? $to_date : '',
		'#date_format' => 'm-d-Y - H:i',  
		'#date_year_range' => '-3:+3',
	);
	
    $form['distributor'] = array(
		'#type' => 'fieldset',
		'#title' => t('Select a distributor'),
		'#weight' => 2,
		'#collapsible' => TRUE, //template_name       
		'#collapsed' => FALSE,
		'#prefix' => '<div class="promo_book-filter-from-date">',
		'#suffix' => '</div>',
	);
	
    $options = promo_book_get_all_promo_names();
    
    //
    //$options[''] = 'ALL';
    $options = array_merge(array('all' => 'ALL'),$options);
     //dpr($options); die;
    $form['distributor']['promo_name'] = array(
		'#type' => 'select',
		'#title' => t('Select Promo Name'), 
		'#options' => $options,
    '#default_value' => isset($_GET['template_name']) ? $_GET['template_name'] : 0,      
		'#attributes' => array('class' => 'container-inline'),
	);
    

    
   
    $form['promo_book_search_downlaod_file'] = array(
		'#type' => 'submit', 
		'#weight' => 10,
		'#value' => t('Download'),
        '#prefix' => '<div class="promo_book_search_downlaod_file">',
		'#suffix' => '</div>',             
	);	
    
  return $form;
}

/**
 * 
 */
function promo_book_download_feed_filter_submit($form,&$form_state){
    //echo "***"; die;
    $from_date = $form_state['values']['from_date'];
    $to_date = $form_state['values']['to_date'];
    $promo_name = $form_state['values']['promo_name'];
    
    $query = db_select('ebay_books_promotions','pb');  
    $query->join('catalog','c','c.isbn13 = pb.promo_isbn');
    $query->join('stock','s','s.isbn13 = c.isbn13');
    
	if($from_date){
	   $from_date =  $from_date." ".$_GET['from_date']['time'];           
	   $from = catalog_generate_uinx_time_stamp($from_date);       
	   //$query->condition('s.dtime',$from,'>');
	}
    
    
	if($to_date){
	    $to_date =  $to_date." ".$_GET['to_date']['time'];
		$to = catalog_generate_uinx_time_stamp($to_date);
	    //$query->condition('s.dtime',$to,'<');
	}
    
    $query->fields('pb',array('promo_name','promo_isbn'));
    $query->condition('pb.status', 'active'); 
   
    if($from && $to){
     $query->where("(promo_days *(86400) + promo_time_from) BETWEEN  $from AND $to " );
    }
    
    if($promo_name != 'all'){        
        $query->condition('pb.promo_name', $promo_name); 
    }
        
  
    $query->fields('s',array('isbn13','source','qty','price','discount','currency'));
    $query->fields('c',array('title','mrp_inr','publisher','listing_price'));
    $query->addExpression('SUM(qty)', 'avail_qty');
    
    
     
   //  dpr($form_state['values']['op']); die;
    if($form_state['values']['op'] == 'Download'){//Check to downlaod XLS file.
      dpq($query);
    
      
      $result = $query->execute();
      
      //print($result);die;
       $header = array('ISBN13','Title','Quantity','Price','Dispatch Time');
       
   	  $xls_content_row = '';
      while($filter_val = $result->fetchObject()){
        $rows = array();
        $rows[]= $filter_val->isbn13;  
        $rows[]= $filter_val->title;   
        $rows[]= $filter_val->avail_qty;
        $rows[]= $filter_val->price; 
        $rows[]= 5;  
                
    	$xls_content_row .= implode("\t", array_values($rows)) . "\r\n";
        
         $info[] = $filter_val;
      }//End of for loop
      dpr($info);
  
    	$xls_content_header = implode("\t", array_values($header));
    	$xls_content = $xls_content_header."\n".$xls_content_row;
    	$filename = 'promo_book_filter_list_'.date("d_m_Y"); 
    	header("Content-type: text/plain; charset=UTF-8");
    	header("Content-Disposition: attachment; filename=$filename");
    	header("Content-Type: application/vnd.ms-excel"); 
    	header("Pragma: no-cache");
    	header("Expires: 0");
    	print $xls_content;
    	exit();   
    	return TRUE;
   }
    
}

/**
 * Implements ajax submit callback of promo_book_search_submit
 *
function promo_book_downlaod_filter_table_view(){
    
  if($_GET['op'] == 'Search' || $_GET['op'] == 'promo_book Download'){
   
    $from_date = $_GET['from_date']['date'];  
    $to_date = $_GET['to_date']['date'];
    $template = $_GET['template_name']; 
    
	//print_r();die;catalog_generate_uinx_time_stamp
	/* if(empty($form_state['values']['template_name']) && empty($form_state['values']['from_date']) && empty($form_state['values']['to_date'])){
		return '<div id="promo_book_search_preview_area"><div>';
	} *

	$query = db_select('promo_book','s');

	if($from_date){
	   $from_date =  $from_date." ".$_GET['from_date']['time'];           
	   $from = promo_book_generate_uinx_time_stamp($from_date);      
	   $query->condition('s.dtime',$from,'>');
	}
    
    
	if($to_date){
	    $to_date =  $to_date." ".$_GET['to_date']['time'];
		$to = promo_book_generate_uinx_time_stamp($to_date);
		$query->condition('s.dtime',$to,'<');
	}
	
    if($template != 0){
      print_r($template);die;
	  	$query->condition('s.source',$template);
	}
    
	$qury = $query;
	$num_rows = $qury->countQuery()->execute()->fetchField();	
    $query->fields('s',array('isbn13','source','qty','price','discount','currency'));
    
    //dpq($query); 
    
    //Make condiational option.
    if($_GET['op'] == 'promo_book Download'){//Check to downlaod XLS file.
      $result = $query->execute();
      $header = array('ISBN13','Title','Quantity','Price','Dispatch Time');
     	while($filter_val = $result->fetchObject()){
        $rows = array();
        $rows[]= $filter_val->isbn13;   
        $rows[]= '-'; 
        $rows[]= promo_book_get_total_available_qty($filter_val->isbn13);   
        $rows[]= promo_book_price_calculator($filter_val->isbn13);   
        $rows[]= 5;   
    	$xls_content_row .= implode("\t", array_values($rows)) . "\r\n";
      }//End of for loop
  
 
    	$xls_content_header = implode("\t", array_values($header));
    	$xls_content = $xls_content_header."\n".$xls_content_row;
    	$filename = 'promo_book_filter_list_'.date("d_m_Y"); 
    	header("Content-type: text/plain; charset=UTF-8");
    	header("Content-Disposition: attachment; filename=$filename");
    	header("Content-Type: application/vnd.ms-excel"); 
    	header("Pragma: no-cache");
    	header("Expires: 0");
    	print $xls_content;
    	exit();   
    	return TRUE;
   }else{
       $query = $query->extend('PagerDefault')->limit(10);    
       $result = $query->execute();
   
    
    	$header = array('ISBN13','Title','Quantity','Price','Dispatch Time');
        
    	while($data = $result->fetchObject()){
    		$is_promo_book = promo_book_check_promo_book($data->isbn13);
        if(!$is_promo_book){
          $rows[] = array(
      		  $data->isbn13,
                '-',
      		  promo_book_get_total_available_qty($data->isbn13),
      		  ceil(promo_book_price_calculator($data->isbn13)),
      		  5
    		  );
        }
    	}
    	
       // Output of table with the paging
       $output = theme('table', array('header' => $header, 'rows' => $rows)) . theme('pager');
      // return $output;
    	
   	return '<div id="promo_book_search_preview_area"><div class="messages status"><h2 class="element-invisible">Status message</h2>'.$num_rows.' records found</div>'.$output.'</div>';
  } 
 }
}*/


/**
 * Get all promo names
 */
function promo_book_get_all_promo_names(){
    $query = db_select('ebay_books_promotions','pbn');
    $query->condition('status','active');
    $query->fields('pbn',array('promo_name','promo_name'));  
    $query->groupBy('promo_name');
    //dpq($query);
    $isbn_redsn = $query->execute()->fetchAllKeyed();   
    return $isbn_redsn;  
}


/**
 * Get all promo names
 *
function promo_book_get_active_list(){
    
}*/


