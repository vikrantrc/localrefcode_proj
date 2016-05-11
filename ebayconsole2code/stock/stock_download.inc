 <?php 


function stock_filter_download_feed(){
   global $user;
   $output='';
   // $output = drupal_get_form('redemption_order_select_form');
   $output .= drupal_render(drupal_get_form('stock_download_feed_filter'));
   $output .= stock_downlaod_filter_table_view();//Get table output format with pagination.
   return $output;
}



/**
 * Implements callback of stock/search
 */
function stock_download_feed_filter($form, &$form_state){
	drupal_add_css(drupal_get_path('module', 'stock') . "/stock.css");
    drupal_add_js(drupal_get_path('module', 'stock') . '/stock.js');
        
    if($_GET['from_date']){
        $from_date = $_GET['from_date']['date'];          
        $date = explode("-",$from_date);        
        $from_date = $date[2].'-'.$date[0].'-'.$date[1];        
        //print_r($date);die;
        $from_date = $from_date ." ". $_GET['from_date']['time'];
    }
     
    if($_GET['to_date']){
        $to_date = $_GET['to_date']['date'];        
        $date = explode("-",$to_date);        
        $to_date = $date[2].'-'.$date[0].'-'.$date[1]; //Y-m-d
        $to_date = $to_date." ".$_GET['to_date']['time'];
    }    
    
  
 	$form = array();
    $form['#method'] = 'GET';
    
    
	$form['from'] = array(
		'#type' => 'fieldset',
		'#title' => t('From Date'),
	//	'#weight' => 0,
		'#collapsible' => TRUE,
		'#collapsed' => FALSE,
		'#prefix' => '<div class="stock-filter-from-date">',
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
	//	'#weight' => 1,
		'#collapsible' => TRUE,
		'#collapsed' => FALSE,
		'#prefix' => '<div class="stock-filter-to-date">',
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
		//'#weight' => 2,
		'#collapsible' => TRUE, //template_name       
		'#collapsed' => FALSE,
		'#prefix' => '<div class="stock-filter-from-date">',
		'#suffix' => '</div>',
	);
	
    $options = stock_get_all_distributors();
    $options[0] = 'ALL';
    $options_price[0] = 'ALL UNMATCHED PRICE';
    $options_price[1] = 'DIFFERENCE MORE THAN 30%';
    $options_price[2] = 'DIFFERENCE LESS THAN 30%';
    //$options = array_merge(array('all' => 'ALL'),$options);
    //  print_r($options);die; 
    $form['distributor']['template_name'] = array(
		'#type' => 'select',
		'#title' => t('Select Distributor'), 
		'#options' => $options,
    '#default_value' => isset($_GET['template_name']) ? $_GET['template_name'] : 0,      
		'#attributes' => array('class' => 'container-inline'),
	);
  
        $form['button-wrap'] = array(
    '#prefix' => '<div class="stock_search_button_wrap">',
    '#suffix' => '</div>',
  );
   
	$form['button-wrap']['stock_search_submit'] = array(
		'#type' => 'submit', 
		'#weight' => 9,
		'#value' => t('Search'),
    '#prefix' => '<div class="stock_search_submit">',
		'#suffix' => '</div>',
	);
    
   
    $form['button-wrap']['stock_search_downlaod_file'] = array(
		'#type' => 'submit', 
		'#weight' => 10,
		'#value' => t('Stock Download'),
    '#prefix' => '<div class="stock_search_downlaod_file">',
		'#suffix' => '</div>',             
	);	
  
    
    $form['button-wrap']['stock_below_10'] = array(
		'#type' => 'submit', 
		'#weight' => 11,
		'#value' => t('Stock Download below 10'),
    '#prefix' => '<div class="stock_search_downlaod_file">',
		'#suffix' => '</div>',             
	);
  $form['button-wrap']['Console_Output'] = array(
		'#type' => 'submit', 
		'#weight' => 12,
		'#value' => t('Console Output'),
    '#prefix' => '<div class="stock_search_downlaod_file">',
		'#suffix' => '</div>',             
	);
      $form['button-wrap']['stock_download_isbn_notincatalog'] = array(
		'#type' => 'submit', 
		'#weight' => 13,
		'#value' => t('Download ISBN for Dummy catalog'),
         '#prefix' => '<div class="stock_search_downlaod_file">',
         '#description' => t('Get isbns which are in stock but not in catalog'),
		'#suffix' => '</div>',             
	);
     $form['button-wrap']['stock_download_unmatched_price'] = array(
		'#type' => 'submit', 
		'#weight' => 13,
		'#value' => t('Stock Download Unmatched Price'),
    '#prefix' => '<div class="stock_search_downlaod_file">',
		'#suffix' => '</div>',             
	);
  	$form['button-wrap']['Range_Price'] = array(
		'#type' => 'select',
		'#title' => t('Range_Price'),
		'#weight' => 14,
		'#collapsible' => TRUE,
		'#collapsed' => FALSE,
    '#options' => $options_price,
     '#prefix' => '<div class="stock_search_downlaod_file">',
		'#suffix' => '</div>',
	);

   
  return $form;
}


/**
 * Implements ajax submit callback of stock_search_submit
 */
function stock_downlaod_filter_table_view(){
     $sla_distb=5;
     if (isset($_GET['op'])) {
  if($_GET['op'] == 'Search' || $_GET['op'] == 'Stock Download'
   || $_GET['op'] == 'Stock Download below 10'||$_GET['op'] == 'Stock Download Unmatched Price'
    || $_GET['op']=='Console Output' ||  $_GET['op']== 'Download ISBN for Dummy catalog'){
  
    $from_date = $_GET['from_date']['date'];  
    $to_date = $_GET['to_date']['date'];
    $template = $_GET['template_name']; 
    
    $Range_Price=$_GET['Range_Price']['value'];
    
   // $subquery=db_select('field_data_field_sla','f_sla_sub');
   // $subquery->join('stock','s_sub','f_sla_sub.entity_id =s_sub.source');
   // $subquery->addExpression('MIN(f_sla_sub.field_sla_value)');
    
    $query = db_select('stock','s');
    $query->leftjoin('catalog','c','c.isbn13 = s.isbn13'); 
    $query->fields('s',array('source','isbn13'));
    $query->execute()->fetchField();
    //$query->join('field_data_field_sla','f_sla','s.source = f_sla.entity_id');
   // $query->join($subquery,'s_sub','s_sub.isbn13=s.isbn13')
    //$query->addExpression('MIN(f_sla.field_sla_value)','min_sla');
   
   if($from_date){
  	   $from_date =  $from_date." ".$_GET['from_date']['time'];      
  	   $from = stock_generate_uinx_time_stamp($from_date);      
  	   //$query->condition('s.dtime',$from,'>');
  	}
  	if($to_date){
  	    $to_date =  $to_date." ".$_GET['to_date']['time'];
  		$to = stock_generate_uinx_time_stamp($to_date);
  		//$query->condition('s.dtime',$to,'<');
  	}
    

    if($template > 0){
		  $query->condition('s.source',$template);
  	}
		
    $query->groupBy('s.isbn13');
    $qury = $query;
    $num_rows = $qury->countQuery()->execute()->fetchField();
    $query->fields('s',array('isbn13','source','qty','price','discount','currency','dtime'));
    $query->fields('c',array('mrp_inr','publisher','listing_price'));
    $query->addExpression('SUM(s.qty)', 'avail_qty');
    $query->addexpression('MAX(s.qty)','max_qty');
    $query->addexpression('MAX(s.dtime)','max_dtime');
  // $query->condition('s.isbn13','9789380349947');
    
   //ACK MEDIA
    $query->condition('c.seller_id','amarchitrakatha2012','!=');
    //zenpublication
    $query->condition('c.seller_id','zenpublication','!=');
    //jaicobooks
    $query->condition('c.seller_id','jaicobooks','!=');
    //bharat.books
    //$query->condition('c.seller_id','bharat.books','!=');
    
    //Make condiational option.
    
    if($_GET['op'] == 'Stock Download'){//Check to downlaod XLS file.
      $result = $query->execute()->fetchAll();
 
      $header = array('ISBN13','Title','Quantity','Price','Dispatch Time');

     	foreach($result as $filter_val){ 
     	//  print_r($filter_val->isbn13);die;
        if($from && $to) {
          if($filter_val->max_qty > 9 && $filter_val->max_dtime > $from && $filter_val->max_dtime < $to ){
            //$minsla = stock_minsla_by_source($newArray[$filter_val->isbn13]);
            
            $rows = array();
            $rows[]= $filter_val->isbn13;   
            $rows[]= '-'; 
            $rows[]= $filter_val->avail_qty;//stock_get_qty_of_isbn($filter_val->isbn13); ////stock_get_qty_of_isbn($filter_val->isbn13);   
            $rows[]= $filter_val->listing_price;   
            $rows[]= 5;    
   	        $xls_content_row .= implode("\t", array_values($rows)) . "\r\n";
          }
        }else {
          if($filter_val->max_qty > 9){
            //$minsla = stock_minsla_by_source($newArray[$filter_val->isbn13]);
            $rows = array();
            $rows[]= $filter_val->isbn13;   
            $rows[]= '-'; 
            $rows[]= $filter_val->avail_qty;//stock_get_qty_of_isbn($filter_val->isbn13); ////stock_get_qty_of_isbn($filter_val->isbn13);   
            $rows[]= $filter_val->listing_price;   
            $rows[]= 5;    
   	        $xls_content_row .= implode("\t", array_values($rows)) . "\r\n";
          }          
        }         
      }//End of for loop
      //print_r($xls_content_row);die;
     	$xls_content_header = implode("\t", array_values($header));
    	$xls_content = $xls_content_header."\n".$xls_content_row;
    	$filename = 'Stock_Download_Greater_10'.date("d_m_Y"); 
    	header("Content-type: text/plain; charset=UTF-8");
    	header("Content-Disposition: attachment; filename=$filename");
    	header("Content-Type: application/vnd.ms-excel"); 
    	header("Pragma: no-cache");
    	header("Expires: 0");
    	print $xls_content;
    	exit();   
    	return TRUE;
   }else if($_GET['op'] == 'Stock Download below 10'){
                
     $result = $query->execute()->fetchAll();
     $header = array('ISBN13','Title','Quantity','Price','Dispatch Time');
     	foreach($result as $filter_val){
     	   if($from && $to) {
            if($filter_val->max_qty < 10 && $filter_val->max_dtime > $from && $filter_val->max_dtime < $to){
              //$minsla = stock_minsla_by_source($newArray[$filter_val->isbn13]);
             
              $rows = array();
              $rows[]= $filter_val->isbn13;   
              $rows[]= '-'; 
              $rows[]= $filter_val->avail_qty;//stock_get_qty_of_isbn($filter_val->isbn13);   
              $rows[]= $filter_val->listing_price;   
              $rows[]= 5;   
              $xls_content_row .= implode("\t", array_values($rows)) . "\r\n";
            }     	    
     	    }else {
     	      if($filter_val->max_qty < 10){
  	          //$minsla = stock_minsla_by_source($newArray[$filter_val->isbn13]);
              $rows = array();
              $rows[]= $filter_val->isbn13;   
              $rows[]= '-'; 
              $rows[]= $filter_val->avail_qty;//stock_get_qty_of_isbn($filter_val->isbn13);   
              $rows[]= $filter_val->listing_price;   
              $rows[]= 5;   
              $xls_content_row .= implode("\t", array_values($rows)) . "\r\n";
            }  
 	    }
      }//End of for loop
      
    	$xls_content_header = implode("\t", array_values($header));
    	$xls_content = $xls_content_header."\n".$xls_content_row;
    	$filename = 'Stock_Download_Below_10'.date("d_m_Y"); 
    	header("Content-type: text/plain; charset=UTF-8");
    	header("Content-Disposition: attachment; filename=$filename");
    	header("Content-Type: application/vnd.ms-excel"); 
    	header("Pragma: no-cache");
    	header("Expires: 0");
    	print $xls_content;
    	exit();   
    	return TRUE;
   }
   else if($_GET['op']=='Console Output'){
  $result= db_query("SELECT s.isbn13 AS isbn13, s.source AS source, 
  s.qty AS qty, s.price AS price,s.source as source,s.dtime as dtime,
  (Select `field_distributor_name_value` FROM `field_data_field_distributor_name`  as ds 
  WHERE  ds.entity_id = s.source) as Distributor,
     c.mrp_inr AS mrp_inr, c.publisher AS publisher,c.title AS title,
     c.author1 AS author
FROM  stock s
 LEFT OUTER JOIN catalog c ON c.isbn13 = s.isbn13 ");
    
     //$query = db_select('stock','s');
//    $query->leftjoin('catalog','c','c.isbn13 = s.isbn13');
//    $query->leftjoin('field_data_field_distributor_name','ds','ds.entity_id = s.source');
// 
//    $query->fields('s',array('isbn13','source','dtime','price','qty'));
//    $query->fields('ds',array('field_distributor_name_value'));
//    $query->fields('c',array('mrp_inr','publisher','title','author1'));
//    $result = $query->execute()->fetchAll();
     $header = array('ISBN13','source','price','qty','mrp_inr','publisher','title','author1');

 
    foreach($result as $filter_val){
      
        if($from && $to) {
        if($filter_val->dtime >= $from && $filter_val->dtime <= $to){
        $rows = array();
        $rows[]= $filter_val->isbn13; 
        $rows[]= $filter_val->Distributor;  
        $rows[]=$filter_val->price;
        $rows[]=$filter_val->qty;
        $rows[]=$filter_val->mrp_inr;
        $rows[]=$filter_val->publisher;
        $rows[]=$filter_val->title;
        $rows[]=$filter_val->author;
 
    
        $xls_content_row .= implode("\t", array_values($rows)) . "\r\n";
    }
    }
    else{
   
        $rows = array();
        $rows[]= $filter_val->isbn13; 
        $rows[]= $filter_val->field_distributor_name_value;  
        $rows[]=$filter_val->price;
        $rows[]=$filter_val->qty;
        $rows[]=$filter_val->mrp_inr;
        $rows[]=$filter_val->publisher;
        $rows[]=$filter_val->title;
        $rows[]=$filter_val->author1;
  
      
    }
      
    }
     	$xls_content_header = implode("\t", array_values($header));
    	$xls_content = $xls_content_header."\n".$xls_content_row;
    	$filename = 'ConsoleOutout_'.date("d_m_Y"); 
    	header("Content-type: text/plain; charset=UTF-8");
    	header("Content-Disposition: attachment; filename=$filename");
    	header("Content-Type: application/vnd.ms-excel"); 
    	header("Pragma: no-cache");
    	header("Expires: 0");
    	print $xls_content;
    	exit();   
    	return TRUE;
    
   }
   //
      else if ($_GET['op'] == 'Stock Download Unmatched Price')  //download records where price difference between stock and catalog  is greater than 30%
   {
  // $subquery = db_select('field_data_field_distributor_name','ds');
  // $subquery->fields('ds',array('field_distributor_name_value'));
  //$subquery->condition('ds.entity_id','s.source','=')
  
    $query = db_select('stock','s');
    $query->join('catalog','c','c.isbn13 = s.isbn13');
    $query->join('field_data_field_distributor_name','ds','ds.entity_id = s.source');
    $query->groupBy('s.isbn13');
    $query->fields('s',array('isbn13','source','dtime','price'));
    $query->fields('ds',array('field_distributor_name_value'));
    $query->fields('c',array('mrp_inr'));
    $query->addExpression('MAX(s.price)', 'max_price_stock');
    $query->addExpression('MAX(c.mrp_inr)', 'max_price_cata');
    $query->addExpression('MAX(s.qty)', 'max_qty');
   // $query->range(0,100);
   
    if($template != 0){
    	 $query->condition('s.source',$template);
}
    $result = $query->execute()->fetchAll();
    $header = array('ISBN13','Stock_MRP','Catalog_MRP','Source','MRP_Difference','Last_Arrived');
    foreach($result as $filter_val){
        $compare_price=0;
        $max_stock = $filter_val->max_price_stock;
        $max_catalog=$filter_val->max_price_cata;
        $abs_max= max($max_stock,$max_catalog);
        $diff_price= abs($max_stock-$max_catalog);
        if($diff_price==0){
          $per_diff_price=0;
        }
        else{$per_diff_price=(($diff_price*100)/$abs_max);
        $per_diff_price=round($per_diff_price, 0, PHP_ROUND_HALF_UP);}
        
        $dtime= $filter_val->dtime;
        $date_dtime=date('m/d/Y',$dtime);
        if( $Range_Price==0){//ALL UNMATCHED PRICE
          if($max_stock!=$max_catalog && $per_diff_price>0){
          $compare_price=1;
          }
        }
        else if($Range_Price==1){//DIFFERENCE MORE THAN 30%
        if($per_diff_price >= 30 ){
         
        $compare_price=1;
        }
      }
        else if($Range_Price==2 ){//DIFFERENCE LESS THAN 30%
        if($per_diff_price<30 && $per_diff_price>0){
             
        $compare_price=1;
        }
      }
   else{ } 
  if($from && $to) {
    if($compare_price==1 && $filter_val->dtime > $from && $filter_val->dtime < $to && $filter_val->max_qty >= 10 ){//&& $filter_val->dtime > $from && $filter_val->dtime < $to //$filter_val->max_qty >10  &&
        $rows = array();
        $rows[]= $filter_val->isbn13; 
        $rows[]= $filter_val->max_price_stock;  
        $rows[]=$filter_val->max_price_cata;
        $rows[]=$filter_val->field_distributor_name_value;
        $rows[]=$per_diff_price.'%';
        $rows[]=$date_dtime;
        $xls_content_row .= implode("\t", array_values($rows)) . "\r\n";
    }
    }
    else{
      if($compare_price==1 && $filter_val->max_qty >=10 ){//&& $filter_val->dtime > $from && $filter_val->dtime < $to //$filter_val->max_qty >10  &&
        $rows = array();
        $rows[]= $filter_val->isbn13; 
        $rows[]= $filter_val->max_price_stock;  
        $rows[]=$filter_val->max_price_cata;
        $rows[]=$filter_val->field_distributor_name_value;
        $rows[]=$per_diff_price.'%';
        $xls_content_row .= implode("\t", array_values($rows)) . "\r\n";
    }
      
    }
    
    }//End of for loop
      	$xls_content_header = implode("\t", array_values($header));
    	$xls_content = $xls_content_header."\n".$xls_content_row;
    	$filename = 'Stock_Price_Difference'.date("d_m_Y"); 
    	header("Content-type: text/plain; charset=UTF-8");
    	header("Content-Disposition: attachment; filename=$filename");
    	header("Content-Type: application/vnd.ms-excel"); 
    	header("Pragma: no-cache");
    	header("Expires: 0");
    	print $xls_content;
    	exit();   
    	return TRUE;
    
    
   }
   else if ($_GET['op'] == 'Download ISBN for Dummy catalog'){
    if($from && $to) {
    $result=db_query(" SELECT t1.isbn13,fd.field_distributor_name_value
        FROM stock t1
        LEFT JOIN catalog t2 ON t2.isbn13 = t1.isbn13 LEFT JOIN field_data_field_distributor_name fd 
        ON fd.entity_id = t1.source
        WHERE  t1.dtime between ".$from." and ".$to." and  t2.isbn13 IS NULL  and t1.qty >9 ORDER BY t1.isbn13 ASC");
  //dpq($result);
  //print_r($result);die;
  }
  else{
     $result=db_query(" SELECT t1.isbn13,fd.field_distributor_name_value
        FROM stock t1
        LEFT JOIN catalog t2 ON t2.isbn13 = t1.isbn13 LEFT JOIN field_data_field_distributor_name fd 
        ON fd.entity_id = t1.source
        WHERE t2.isbn13 IS NULL  and t1.qty >9 ORDER BY t1.isbn13 ASC");
  //dpq($result);
    
  }
     $header = array('ISBN13','SOURCE');
     foreach($result as $filter_val){
    $rows = array();
    $rows[]= $filter_val->isbn13; 
    $rows[]= $filter_val->field_distributor_name_value; 
    $xls_content_row .= implode("\t", array_values($rows)) . "\r\n";
    }
    $xls_content_header = implode("\t", array_values($header));
    $xls_content = $xls_content_header."\n".$xls_content_row;
    $filename = 'Dummycatalog_Isbn'.date("d_m_Y"); 
    header("Content-type: text/plain; charset=UTF-8");
    header("Content-Disposition: attachment; filename=$filename");
    header("Content-Type: application/vnd.ms-excel"); 
    header("Pragma: no-cache");
    header("Expires: 0");
    print $xls_content;
    exit();   
    return TRUE;
 }
   
   //
   
   
   
   else{
            
            $query = $query->extend('PagerDefault')->limit(10);    
            $result = $query->execute();
            $header = array('ISBN13','Title','Quantity','Price','Dispatch Time');
        
    	while($data = $result->fetchObject()){
    		$is_promo_book = stock_check_promo_book($data->isbn13);
        if(!$is_promo_book){
          $rows[] = array(
      		  $data->isbn13,
                '-',
      		  $data->avail_qty,
      		  ceil($data->listing_price),
      		  $sla_distb,
    		  );
        }
    	}
      $output = theme('table', array('header' => $header, 'rows' => $rows)) . theme('pager');
   	  return '<div id="stock_search_preview_area"><div class="messages status"><h2 class="element-invisible">Status message</h2>'.$num_rows.' records found</div>'.$output.'</div>';
  }
 
 }
}
}

function stock_download_assig_sla_by_nid()
{
    $sla_distb_ar = array('2519','2523','2577'.'2562','2591','2543','2588','2578','2564',
                            '2602','2579','2539','2609');
      $source=$filter_val->source;
    if (in_array($source,$sla_distb_ar)){
    $sla_distb = 10;
    }
}

function stock_download_min_sla() {
  global $base_path;
  drupal_set_message('Before start downloading the list, please make sure that you have inserted your new isbn list into table which is available from stock download(>=10). To do it, click <a href="'.$base_path.'stock/insert/minslaisbn"> here </a>.');
   
  $num_rows = db_select('stock_download_minsla','ptr')->fields('ptr')->execute()->rowCount();  
  if($num_rows>0) {  
    $rem_range = $avg_range = 0;
    $avg_range = floor($num_rows / 4);
    $rem_range = $num_rows % 4;  
    
    $form = array();
    $form['hd'] = array(
    '#markup' => t('Total avilable records of SLA is <strong>'.$num_rows.'</strong>'),
    '#prefix'  => '<div id="stock_excel_email_wrap">',
    '#suffix'  => '</div>',
    ); 
  
    if($num_rows > 20000) {
      $form['avg'] = array(
      '#markup' => t('<p>Each part of below links contain <strong>'.$avg_range.'</strong> records</p>'),
      '#prefix'  => '<div id="stock_excel_email_wrap">',
      '#suffix'  => '</div>',
      );     
      $form['first'] = array(
      '#markup' => l('Download first part of min SLA ','stock/download/range/1/'.$avg_range),
      '#prefix'  => '<div id="stock_excel_email_wrap">',
      '#suffix'  => '</div>',
      );  
      $form['second'] = array(
      '#markup' => l('Download second part of min SLA ','stock/download/range/'.($avg_range+1).'/'.(2*$avg_range)),
      '#prefix'  => '<div id="stock_excel_email_wrap">',
      '#suffix'  => '</div>',
      );
      $form['third'] = array(
      '#markup' => l('Download third part of min SLA ','stock/download/range/'.((2*$avg_range)+1).'/'.(3*$avg_range)),
      '#prefix'  => '<div id="stock_excel_email_wrap">',
      '#suffix'  => '</div>',
      );
      $form['fourth'] = array(
      '#markup' => l('Download last part of min SLA ','stock/download/range/'.((3*$avg_range)+1).'/'.((4*$avg_range)+$rem_range)),
      '#prefix'  => '<div id="stock_excel_email_wrap">',
      '#suffix'  => '</div>',
      );
    }else {
      $form['fourth'] = array(
      '#markup' => l('Download min SLA ','stock/download/range/1/'.$num_rows),
      '#prefix'  => '<div id="stock_excel_email_wrap">',
      '#suffix'  => '</div>',
      );
    }
    return $form;
  }else {
    return 'No data available from stock_download_minsla Table. Please insert isbn list first !';
  }
}

function stock_download_min_sla_by_range() {

  if(is_numeric(arg(3)) && is_numeric(arg(4))) {  
    
    /*$frm =  mktime(10, 00,00, date('m',time()),date('d',time()), date('Y',time()));
    $to =  mktime(22, 00,00, date('m',time()),date('d',time()), date('Y',time()));*/
    
    $query = db_select('stock','s');
    $query->join('stock_download_minsla','sla','sla.isbn13 = s.isbn13'); 
    $query->condition('s.qty',10,'>=');
    $query->condition('sla.sno', array(arg(3),arg(4)), 'BETWEEN');
    $query->fields('s',array('isbn13','source','qty'));
    $result = $query->execute()->fetchAll();
    $newArray = array();
    foreach($result as $entity) {
      if(!isset($newArray[$entity->isbn13])){
        $newArray[$entity->isbn13] = array();
      }
      $newArray[$entity->isbn13][ $entity->qty] = $entity->source;
    }
    // print_r($newArray);die;
    $header = array('ISBN13','OLD MIN SLA','NEW MIN SLA','all_sla');
    $final_arr = array();
    foreach($newArray as $k=>$v) {
       
     $sla_dis='';
     $all_sla_add='';
    // $items = array();
    // $count = 0;
     $sla_dis=array();
   foreach($v as $s=>$t){
     //print_r($v);die;  
    $slget=get_sla_from_discount_table($t,$s);//$t=source,$s=quantity
       
    if($slget!='' || $slget!= null || !empty($slget)){
        $sla_dis[] = $slget; 
        
    }
  
    
    // array_push($sla_dis,get_sla_from_discount_table($t,$s)); 
  // print_r($sla_dis);die;
   // $items[$count++] = array(get_sla_from_discount_table($t,$s));
    $all_sla_add .= get_sla_from_discount_table($t,$s)." | ";
    //  print_r($all_sla_add);die;
    //if($sla_dis==''){$sla_dis=0;}
    // $sla_add  .= implode("\t", array_values($sla_dis)) . "\r\n";
    
      
   }
  
 
     if(  $sla_dis!=''  && !empty($sla_dis) && $sla_dis!=NULL){
        $output_sla=min($sla_dis);
         
     }
 
  
  else{$output_sla='';}
 
    

   
   
      //if(is_array($v[0])) {  
          
   
        $nwqry =  db_select('field_data_field_sla','sla_ptr')
        ->fields('sla_ptr',array('field_sla_value'))
        ->condition('entity_id',$v,'IN');
        $nwqry->addExpression('MIN(field_sla_value)','minsla');
        $result = $nwqry->execute()->fetchAll(); 
        $rows = array();
        $rows[]= $k;   
        $rows[]= $result[0]->minsla;   
        $rows[]=$output_sla;
        $rows[]=$all_sla_add;
        $xls_content_row .= implode("\t", array_values($rows)) . "\r\n";
     // }     
    }   //print_r($output_sla);die; 
    $xls_content_header = implode("\t", array_values($header));
    $xls_content = $xls_content_header."\n".$xls_content_row;
    $filename = 'Stock_min_sla_list_'.date("d_m_Y").'_'.arg(3).'-'.arg(4); 
    header("Content-type: text/plain; charset=UTF-8");
    header("Content-Disposition: attachment; filename=$filename");
    header("Content-Type: application/vnd.ms-excel"); 
    header("Pragma: no-cache");
    header("Expires: 0");
    print $xls_content;
    exit();   
    return TRUE;
  }
  else {
    return 'One of the range is not a numeric value. Please check the arguments :( ';
  }
}

/**
 * Get list of isbns after download the stock. Then insert into table to find min SLA
 */
function stock_insert_isbn_to_cal_minsla() {
  $form['insert_isbn'] = array(
		'#type' => 'file',
		'#title' => t('Select a file which contains list of isbn numbers'),
		'#size' => 35,
    '#description' => t('File with extension of xls or xlsx are the only valid formats.'),
	);
	$form['read_excel_file'] = array(
		'#type' => 'submit', 
		'#value' => t('Insert ISBN'),
		
	);
	return $form;
}

/**
 * Implements hook_validate
 */
function stock_insert_isbn_to_cal_minsla_validate($form , &$form_state){
	$file = file_save_upload('insert_isbn', array('file_validate_extensions' => array('xls xlsx'),));
	require_once 'excel_reader2.php';
        
	if ($file) {
      if ($file = file_move($file, 'public://')) {
        $form_state['values']['file'] = $file;
      }else {
        form_set_error('file', t('Failed to write the uploaded file the site\'s file folder.'));
      }
    }
}

/**
 * Implements hook_submit
 */
function stock_insert_isbn_to_cal_minsla_submit($form , &$form_state){ 

	global $base_path;
  include 'simple/simplexlsx.class.php';
  require_once 'excel_reader2.php';
    
  $file = $form_state['values']['file'];
  $file->status = FILE_STATUS_PERMANENT;
  $filepath = drupal_realpath($file->uri);
  
  ###################mapping of excel readers###################
  $ext = pathinfo($filepath, PATHINFO_EXTENSION);
  ##############################################################
  $multi_col = 0;
  if($ext == 'xls'){
    $xlsx_rw = 2;
    $data = new Spreadsheet_Excel_Reader($filepath);
    $numrows = count($data->sheets[0]['cells']);
    $numcols = $data->sheets[0]['numCols'];  
    if($numcols >1){
      $multi_col = 1;
      goto end;
    }
    $result = db_truncate('stock_download_minsla')->execute();
    //xls start from zero 0
    for($row = 2; $row <= $numrows; $row++){
      for ($col = 1; $col <= $numcols; $col++) {
        $output[$row][$col] = $data->val($row, $col, $sheet = 0);  
      }          
      //print_r($output[$row]);die;
    }
  }else if($ext == 'xlsx'){
    $xlsx_rw = 1;
    $data = new SimpleXLSX($filepath);
    $test =  list($numcols, $numrows) = $data->dimension();
    $info = $data->rows();
    if($numcols >1){
      $multi_col = 1;
      goto end;
    }
    $result = db_truncate('stock_download_minsla')->execute();
    //xlsx start from zero 0
    for($r = 1; $r < $numrows; $r++){
      $row = $r;  
      foreach($info[$r] as $col => $cols){
        $output[$row][$col+1] = $cols;      
      }         
      // print_r($output[$row]);die;
    }
  }
  
  foreach($output as $k=>$v) {
     $query=db_insert('stock_download_minsla')
      ->fields(array(
      'isbn13' => $v[1],
      ));
      $res = $query->execute(); 
  }
  $num_rows = db_select('stock_download_minsla','ptr')->fields('ptr')->execute()->rowCount();     
  drupal_set_message($num_rows.' Records are inserted successfully ! Click <a href="'.$base_path.'stock/download/minsla"> here </a> to download SLA');
  
  end:
  if($multi_col == 1) {
     drupal_set_message('File should not contain multiple column :(','error');
  }
}
/**
 * Download last upload list of all sources on current day
 */
function stock_download_last_uploded(){
  $header=array('Source','Source Code','Last Uploded On','UP/DOWN','Quantity Status');
  $db_last_upl = db_select('stock','s');
  $db_last_upl->join('field_data_field_distributor_name' ,'dn','dn.entity_id = s.source');
  $db_last_upl->fields('dn',array('field_distributor_name_value'));
  $db_last_upl->fields('s',array('source'));
  $db_last_upl->distinct();
  //$db_last_upl->range(0,50);
  $sources= $db_last_upl->execute()->fetchAll();
 // print_r($sources);die;
  $newArray = array();
    foreach($sources as $entity) {
      if(!isset($newArray[$entity->source])){
        $newArray[$entity->field_distributor_name_value] = array($entity->source);
      }
      
    }
   // print_r($newArray);die;
  foreach($newArray as $ke=>$sc){
    $status_source='';
$get_up_status=db_select('stock','sat');
$get_up_status->fields('sat',array('qty','source'));
$get_up_status->condition('sat.source',$sc,'IN');
$get_up_status->condition('qty',0,'!=');
//$status_up=$get_up_status->execute()->fetchAll();
$number_of_rows = $get_up_status->execute()->rowCount();
$all_val=$get_up_status->execute()->fetchAll();
//print_r($all_val);die;
if($number_of_rows==0){
  $status_source='DOWN';
  $status_quantity='(All quantity 0 in tool)';
  
}
else{ 
  
  $status_source='UP';
  $status_quantity='(Currently-'.$number_of_rows.'-Records have non zero quantity)'; 
  
  
}
 

 

 
  $date_upl=db_select('stock','sa');
  $date_upl->fields('sa',array('dtime','source'));
//  $date_upl->range(0,1);
  $date_upl->condition('sa.source',$sc,'IN');
  $date_upl->addexpression('MAX(sa.dtime)','max_dtime');
  $list= $date_upl->execute()->fetchAll();
 
   $time=$list[0]->max_dtime;
  $datetime=date('Y-m-d ',$time);
   $rq=explode("-",$status_quantity);
    $qu=$rq[1];
 
 // echo $time.'dsd';
 //print_r($datetime);die;9780548060896
    foreach($list as $opt){
        
   if($opt->source==2533 && $number_of_rows < 20000 && $number_of_rows!=0 ){
   $status_quantity = '(Currently-'.$number_of_rows.'-Records have non zero quantity)'.' *SOMETHING CAN BE WRONG*';
  }
    if($opt->source==2634 && $number_of_rows < 2400 && $number_of_rows!=0 ){
   $status_quantity = '(Currently-'.$number_of_rows.'-Records have non zero quantity)'.' *SOMETHING CAN BE WRONG';
  }
   
     //print_r($opt);die;
      $rows=array();
      $rows[]=$ke;
      $rows[]=$opt->source;
      $rows[]=$datetime;
      $rows[]=$status_source;
      $rows[]=$status_quantity;
    }
    $xls_content_row .= implode("\t", array_values($rows)) . "\r\n";
  }
  $xls_content_header = implode("\t", array_values($header));
  $xls_content = $xls_content_header."\n".$xls_content_row;
  $filename = 'Last_Uploded_List'.date("d_m_Y"); 
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
 * Implements ISBN qc
 */
function stock_isbn_search_results($isbnqc){
    $isbnqc_get=arg(4);
    $down_stat=arg(9);
    if($down_stat == 1){
    $isbnqc_get=arg(8);
    $length=strlen($isbnqc_get);
    $isbn = (chunk_split($isbnqc_get, 13,"\n")); 
    $isbnqcex=explode("\n",$isbn);
    }
    else{ $isbnqcex=explode("\n",$isbnqc_get);}
 
 foreach($isbnqcex as $isbnqc){
 
    $isbnqci =  trim(trim($isbnqc,"\x00..\x1F"));
  $header = array('ISBN','SOURCE','Quantity',
  'MRP in Stock','Title','mrp inr catalog','listing price catalog', 'Last Uploded(m-d-Y | H:i:s)');
  $get_isbn_detail=db_select('stock','s');
  $get_isbn_detail->join('field_data_field_distributor_name','dn','dn.entity_id = s.source');
  $get_isbn_detail->leftjoin('catalog','c','c.isbn13 = s.isbn13');
  $get_isbn_detail->fields('c',array('title','mrp_inr','listing_price'));
  $get_isbn_detail->fields('dn',array('field_distributor_name_value'));
  $get_isbn_detail->fields('s',array('source','qty','dtime','isbn13','price'));
  $get_isbn_detail->condition('s.isbn13',$isbnqci,'=');
  $get_qc=$get_isbn_detail->execute()->fetchAll();
    foreach($get_qc as $rec) {
    if(!isset($newArray[$rec->isbn13])){
    $newArray[$rec->dtime] = array($rec->isbn13,$rec->dtime,$rec->field_distributor_name_value,
    $rec->qty,$rec->price,$rec->title,$rec->mrp_inr,$rec->listing_price);
    }
    }
  //$old_mrp=$newArray->mrp_inr;
    $row = array();
  
    $old_mrp=($newArray[$rec->dtime][4]); 
    foreach($newArray as $DTIME=>$dta){
    $time=$dta[1];
    $datetime=date('m-d-Y || H:i:s',$time);
	$row = array();

    $row[]=$dta[0];
    $row[]=$dta[2];
  	$row[] = "<input type='number' id='qty_over' value='".$dta[3]."'/>" ;
    $row[]=$dta[4];
    $row[]=$dta[5];
    $row[]=$dta[6];
    $row[]=$dta[7];
	$row[] = $datetime;
    $rows[] = $row;
    $rows_all .=$row;
     
    if($down_stat == 1){
        $row=str_replace("<input type='number' id='qty_over' value='".$dta[3]."'/>",$dta[3],$row);
      $xls_content_row .= implode("\t", array_values($row)) . "\r\n";  
   } 
  
     }
   
 }
// print_r($rows_all);die;  
    function arrayUnique($array) {
     
    $input = array_map("unserialize", array_unique(array_map("serialize", $array)));
    foreach ( $input as $key => $value ) {
    is_array($value) and $input[$key] = arrayUnique($value);
     }
    return $input;
     }
     if($down_stat==1){
     $xls_content_header = implode("\t", array_values($header));
     $xls_content_row_dup= explode("\n", $xls_content_row);
    
    // print_r($xls_content_row);die;
     $xls_content_row_clean=arrayUnique($xls_content_row_dup);
     foreach($xls_content_row_clean as $xls_content_row_clean_dwn){
     $xls_contents.= $xls_content_row_clean_dwn;
    }
     foreach($xls_contents as $key=>$val){
     //print_r($val[0]); 
     $sort_col[$key] = $row[$val[0]];
     
   }
    array_multisort($sort_col, SORT_ASC, $xls_contents);
    //print_r($xls_contents);die;
    
  
    $xls_content= $xls_content_header."\n".$xls_contents;
    $filename = 'QC_RESULTS-'.date("d_m_Y"); 
    header("Content-type: text/plain; charset=UTF-8");
    header("Content-Disposition: attachment; filename=$filename");
    header("Content-Type: application/vnd.ms-excel"); 
    header("Pragma: no-cache");
    header("Expires: 0");
    print $xls_content;
    exit();   
    return TRUE;
    }
    // $newrow= array_multisort($rows[0], SORT_ASC);
  
  function array_sort_by_column(&$arr, $col, $dir = SORT_ASC) {
    $sort_col = array();
    foreach ($arr as $key=> $row) {
        $sort_col[$key] = $row[$col];
    }

    array_multisort($sort_col, $dir, $arr);
}
 
   foreach($rows_all as $key=>$val){
     //print_r($val[0]); 
     $sort_col[$key] = $row[$val[0]];
     
   }
   array_multisort($sort_col, SORT_ASC, $rows_all);
// $new_rows=explode("\t",$xls_contents);
   $rows = array_map('unserialize', array_unique(array_map('serialize', $rows)));
 //print_r($new_rows);die;
 
   $downl=1;
   if($rows){
   $form['markup']=array(
    '#type' => 'markup',
    '#markup' => '<h2><a href="sales/download/isbn_qc/result/'.$isbnqc_get.'/'.$downl.'">Click here to download Results(For Multiple ISBN download this)</a></h2>',
    );
    }
    $form['myselector'] = array (
    '#type' => 'tableselect',
    '#title' => 'My Selector',
    '#header' => $header,
    '#options' => $rows,
    ); 
    if(!$rows){
    $form['no_rec_promo'] = array ( 
    '#markup' => "No records found",
    );
    } 
    $form['page_shown'] = array (        
    '#markup' => theme("pager"),    
    );  
    $form['promo_group'] = array (       
    '#markup' => l('Back', 'stock/isbn/qc'),        
    ); 
   return $form; 
 
}