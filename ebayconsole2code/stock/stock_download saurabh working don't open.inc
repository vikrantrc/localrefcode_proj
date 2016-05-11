 <?php 


function stock_filter_download_feed(){
   global $user;
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
		'#weight' => 0,
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
		'#weight' => 1,
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
		'#weight' => 2,
		'#collapsible' => TRUE, //template_name       
		'#collapsed' => FALSE,
		'#prefix' => '<div class="stock-filter-from-date">',
		'#suffix' => '</div>',
	);
	
    $options = stock_get_all_distributors();
    $options[0] = 'ALL';
    //$options = array_merge(array('all' => 'ALL'),$options);
    //  print_r($options);die; 
    $form['distributor']['template_name'] = array(
		'#type' => 'select',
		'#title' => t('Select Distributor'), 
		'#options' => $options,
    '#default_value' => isset($_GET['template_name']) ? $_GET['template_name'] : 0,      
		'#attributes' => array('class' => 'container-inline'),
	);
    
	$form['stock_search_submit'] = array(
		'#type' => 'submit', 
		'#weight' => 9,
		'#value' => t('Search'),
    '#prefix' => '<div class="stock_search_submit">',
		'#suffix' => '</div>',
	);
    
   
    $form['stock_search_downlaod_file'] = array(
		'#type' => 'submit', 
		'#weight' => 10,
		'#value' => t('Stock Download'),
    '#prefix' => '<div class="stock_search_downlaod_file">',
		'#suffix' => '</div>',             
	);	
    
    $form['stock_below_10'] = array(
		'#type' => 'submit', 
		'#weight' => 11,
		'#value' => t('Stock Download below 10'),
    '#prefix' => '<div class="stock_search_downlaod_file">',
		'#suffix' => '</div>',             
	);
     $form['stock_download_unmatched_price'] = array(
		'#type' => 'submit', 
		'#weight' => 12,
		'#value' => t('Stock Download Unmatched Price'),
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
  if($_GET['op'] == 'Search' || $_GET['op'] == 'Stock Download' || $_GET['op'] == 'Stock Download below 10'||$_GET['op'] == 'Stock Download Unmatched Price'){
  
    $from_date = $_GET['from_date']['date'];  
    $to_date = $_GET['to_date']['date'];
    $template = $_GET['template_name']; 
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
    

    
    if($template != 0){
  	  	//$query->condition('s.source',$template);
  	}
    $query->groupBy('s.isbn13');
    $qury = $query;
    $num_rows = $qury->countQuery()->execute()->fetchField();
    $query->fields('s',array('isbn13','source','qty','price','discount','currency','dtime'));
    $query->fields('c',array('mrp_inr','publisher','listing_price'));
    $query->addExpression('SUM(s.qty)', 'avail_qty');
    $query->addexpression('MAX(s.qty)','max_qty');
    
    //Make condiational option.
    
    if($_GET['op'] == 'Stock Download'){//Check to downlaod XLS file.
     
      $result = $query->execute()->fetchAll();
      //print_r($result);die;
      $header = array('ISBN13','Title','Quantity','Price','Dispatch Time');
     	foreach($result as $filter_val){ 
        if($from && $to) {
          if($filter_val->max_qty > 9 && $filter_val->dtime > $from && $filter_val->dtime < $to ){
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
    	$filename = 'Stock_filter_list_'.date("d_m_Y"); 
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
     // print_r($result);die;
      $header = array('ISBN13','Title','Quantity','Price','Dispatch Time');
     	foreach($result as $filter_val){
     	   if($from && $to) {
            if($filter_val->max_qty < 10 && $filter_val->dtime > $from && $filter_val->dtime < $to){
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
   //
      else if ($_GET['op'] == 'Stock Download Unmatched Price')  //download records where price difference between stock and catalog  is greater than 30%
   {
    \  //(Select `field_distributor_name_value` FROM `field_data_field_distributor_name`  as ds 
  // WHERE  ds.entity_id = s.source) as Distributor,
  $subquery =db_select('field_data_field_distributor_name','ds')
  $subquery->fields('ds',array('field_distributor_name_value'));
  //$subquery->condition('ds.entity_id','s.source','=')
  
    $query = db_select('stock','s');
    $query->join('catalog','c','c.isbn13 = s.isbn13');
    $query->join ($subquery,'dss','dss.entity_id=s.source')
    $query->groupBy('s.isbn13');
    $query->fields('s',array('isbn13','source','dtime','price'));
    $query->fields('c',array('mrp_inr'));
    //$query->addExpression('SUM(s.qty)', 'avail_qty');
    //$query->range(0,10);
    $query->addExpression('MAX(s.price)', 'max_price_stock');
    $query->addExpression('MAX(c.mrp_inr)', 'max_price_cata');
   
    $compare_price=0;
  
 
  //  $query->condition('s.dtime',array($from,$to),'BETWEEN');
    $result = $query->execute()->fetchAll();
      //print_r($result);die;
      $header = array('ISBN13','Stock_Price','Catalog_price','Source','Price_Difference');
     	foreach($result as $filter_val){
     	    $max_stock = $filter_val->max_price_stock;
            $max_catalog=$filter_val->max_price_cata;
            $abs_max= max($max_stock,$max_catalog);
            $diff_price= abs($max_stock-$max_catalog);
            $per_diff_price=(($diff_price*100)/$abs_max);
          if($max_stock!=$max_catalog){
            
            $compare_price=1;
            
          }
            if($compare_price==1){//&& $filter_val->dtime > $from && $filter_val->dtime < $to //$filter_val->max_qty >10  &&
                $rows = array();
                $rows[]= $filter_val->isbn13; 
                $rows[]= $filter_val->price;  
                $rows[]=$filter_val->mrp_inr;
                $rows[]=$filter_val->field_distributor_name_value;
                $rows[]=$per_diff_price.'%';
       	        $xls_content_row .= implode("\t", array_values($rows)) . "\r\n";
            }
          
            
      }//End of for loop
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
      '#markup' => l('Download first part of min SLA ','stock/download/range/0/'.$avg_range),
      '#prefix'  => '<div id="stock_excel_email_wrap">',
      '#suffix'  => '</div>',
      );  
      $form['second'] = array(
      '#markup' => l('Download second part of min SLA ','stock/download/range/'.$avg_range.'/'.$avg_range),
      '#prefix'  => '<div id="stock_excel_email_wrap">',
      '#suffix'  => '</div>',
      );
      $form['third'] = array(
      '#markup' => l('Download third part of min SLA ','stock/download/range/'.(2*$avg_range).'/'.$avg_range),
      '#prefix'  => '<div id="stock_excel_email_wrap">',
      '#suffix'  => '</div>',
      );
      $form['fourth'] = array(
      '#markup' => l('Download last part of min SLA ','stock/download/range/'.(3*$avg_range).'/'.($avg_range+$rem_range)),
      '#prefix'  => '<div id="stock_excel_email_wrap">',
      '#suffix'  => '</div>',
      );
    }else {
      $form['fourth'] = array(
      '#markup' => l('Download min SLA ','stock/download/range/0/'.$num_rows),
      '#prefix'  => '<div id="stock_excel_email_wrap">',
      '#suffix'  => '</div>',
      );
    }
    return $form;
  }
}

function stock_download_min_sla_by_range() {

  if(is_numeric(arg(3)) && is_numeric(arg(4))) {  
    
    /*$frm =  mktime(10, 00,00, date('m',time()),date('d',time()), date('Y',time()));
    $to =  mktime(22, 00,00, date('m',time()),date('d',time()), date('Y',time()));*/
    
    $query = db_select('stock','s');
    $query->join('stock_download_minsla','sla','sla.isbn13 = s.isbn13'); 
    $query->condition('s.qty',10,'>=');
    $query->range(arg(3),arg(4));
   // $query->condition('s.dtime', array($frm, $to), 'BETWEEN');
    $query->fields('s',array('isbn13','source'));
    $result = $query->execute()->fetchAll();
    
    $newArray = array();
    foreach($result as $entity) {
      if(!isset($newArray[$entity->isbn13])){
        $newArray[$entity->isbn13] = array();
      }
      $newArray[$entity->isbn13][] = $entity->source;
    }
    
    $header = array('ISBN13','MIN sla');
    $final_arr = array();
    foreach($newArray as $k=>$v) {
      if(is_array($v)) {
        $nwqry =  db_select('field_data_field_sla','sla_ptr')
        ->fields('sla_ptr',array('field_sla_value'))
        ->range(0,1)
        ->condition('entity_id',$v,'IN');
        $nwqry->addExpression('MIN(field_sla_value)','minsla');
        $result = $nwqry->execute()->fetchAll(); 
        //$final_arr[$k] = $result[0]->minsla;     
        $rows = array();
        $rows[]= $k;   
        $rows[]= $result[0]->minsla;   
        $xls_content_row .= implode("\t", array_values($rows)) . "\r\n";
      }
    }  
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

