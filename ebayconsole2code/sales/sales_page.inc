<?php
 
/**
 * Display and assign sales orders 
 * Using Quick tab custom code
 */
function sales_auto_assign_order_reassignment(){
	$_SESSION['sales_invoice_next'] = '';
 drupal_add_css(drupal_get_path('module', 'sales') . '/sales.css');
 drupal_add_js(drupal_get_path('module','sales').'/sales.js'); 
 //Fetch all distributor who has assigned order
 if($distributor_tabs = sales_get_all_order_assigned_tabs()){ 
    foreach($distributor_tabs as $tabs_id){
      $distributor_id = $tabs_id->distrbutor_id;
      //Get distributor name by id
      $title = stock_get_distributor_name_by_id($distributor_id) ;
      $title =  ($title == '0') ? 'Market': $title;
      $mytabs[] = array(
        'title'   => $title,
        //'contents' => sales_show_table_structure_assign_orders($distributor_id),
        'weight'  => 0,
        'teaser' => FALSE,
        'hide_title' => TRUE,
        'path' => 'sales/order/get_assignment/'.$distributor_id,
        'type' => 'callback',
      );       
    }
    
    $tabs = $mytabs;
    $quicktabs_options = array(
       'style'   => 'Navlist',
       'ajax'    => TRUE,
       'hide_empty_tabs' => TRUE,
       //'default_tab' => $title,       
    );
    $quicktabs_name = 'dynamic_quicktabs_menu';
 
    $quicktabs = quicktabs_build_quicktabs($quicktabs_name, $quicktabs_options, $tabs);
     
    //return '<div id="assignment_quicktabs">'.$quicktabs.'</div>';
    return drupal_render($quicktabs).'<div id="backs"><a href="/ebayconsole2/sales/activate/deactivate/batch">BACK</a></div>'.'<div class="next-popup-wrap">
    <p class="desc"> By clicking this all orders will get freezed for procurement to continue click Ok</p>
    <p class="action"><span class="flL"><a href="#">[ Cancel ]</a></span><span class="flR"><a href="/ebayconsole2/sales/invoice/next">[ Ok ]</a></span></p>
    </div><div id ="next"><a class="next-popup" href="#">NEXT</a></div>';
    //current status download
    //<div class="down_cur"><a href="/ebayconsole2/sales/current/status/download">Download current status in excel</a></div>'
  }else {
		$_SESSION['sales_invoice_next'] = 'updated';
		drupal_goto('sales/invoice/next');
	}//End If
}
function sales_invoice_listing(){
	$header = array('Source List', 'Assigned Books', 'PO ID.', 'Updated time','Download');
	if($_SESSION['sales_invoice_next'] =='') {
		//Update all invoice numbers
        $cur_time= mktime(9, 00,00, date('m',time()),date('d',time()), date('Y',time()));
		$update_freeze = db_update('ebay_books_order_assigned');
		$update_freeze->fields(array('status' => 'freezed','dtime' => $cur_time));
        $update_freeze->condition('status','assigned');
		$update_freeze->execute(); 	
		//Update invoice no same for a particular distributor
		//"SELECT distrbutor_id,invoice_no,count(distrbutor_id) FROM `ebay_books_order_assigned` group by  distrbutor_id having count(distrbutor_id) >1")
		$select_source = db_select('ebay_books_order_assigned','eb');
		$select_source->fields('eb',array('distrbutor_id'));
		$select_source->groupBy('distrbutor_id');
		$select_source->condition('status','freezed');
		$result_set = $select_source->execute()->fetchAll();

		foreach($result_set as $rec_set){
		$max_inv_in = db_query("SELECT MAX(invoice_no) as max_inv FROM ebay_books_order_assigned")->fetchField();
		$source_updated = db_update('ebay_books_order_assigned');     
		$source_updated->condition('distrbutor_id',$rec_set->distrbutor_id,'=');
		$source_updated->condition('status','freezed');
		$source_updated->fields(array('invoice_no' => $max_inv_in+1));
		$source_updated->execute(); 
		}
	}
	//Print the invoice listing page
  $query = db_select('ebay_books_order_assigned','oa');
  $query->leftjoin('field_data_field_distributor_name','ds','ds.entity_id = oa.distrbutor_id');
  $query->fields('ds',array('field_distributor_name_value','entity_id'));
  $query->fields('oa',array('qty','price','order_id','order_assigned_id','invoice_no','dtime')); 
  $query->addExpression('SUM(oa.qty)', 'qty');
  $query->groupby('oa.distrbutor_id');
  $query->condition('oa.status','to_be_procured_waiting','!=');
  $query->condition('oa.status','procured_waiting','!=');
  $query->condition('oa.status','assigned_waiting','!=');
  $query->condition('oa.status','freezed_waiting','!=');
  
  $query = $query->extend('PagerDefault')->limit(10);
  $num_rows = $query->countQuery()->execute()->fetchField();
  $results = $query->execute();
 // print_r($num_rows);die;
    if($num_rows==0){
    drupal_set_message("No Batch Is Active Please Activate one batch");
    drupal_goto('sales/activate/deactivate/batch');
    }
 

 
  $today_date= date("Ymd");
   foreach($results as $invoice_list){

    $curr_timestamp= time();
		$order_inv=$invoice_list->order_id;
		$source_id=$invoice_list->entity_id;
		$source_inv=$invoice_list->field_distributor_name_value;
		$order_assigned_inv=$invoice_list->order_assigned_id;
		$invoice_no=$invoice_list->invoice_no;
		if($source_inv == ''){
			 $source_inv='Market';
     }
    $row = array();
    $row[] = $source_inv;
    $row[] = $invoice_list->qty;
    $row[] = $invoice_no;
    $row[] = date('m-d-Y',$invoice_list->dtime);
    $row[] = '<a href="/ebayconsole2/sales/order/invoice/download/'.$source_id.'/'.$invoice_no.'" >Download Purchase Order</a>';
    $rows[] = $row; 
    }
		$_SESSION['sales_invoice_next'] = 'updated'; 
  return '<div><div id="nextmain"><strong><a href="/ebayconsole2/sales/order/invoice/download/all/all">Click here to download all Purchase Orders</a></strong></div><div id="nextmain2"><strong><a href="/ebayconsole2/sales/next/to_be_procured">NEXT</a></strong></div>'.theme('table', array('header' => $header, 'rows' => $rows)).theme('pager').'</div>'; 
 }

/**
 * Updates status from freezed to to be procured
 * 
 */
function sales_to_be_procured(){
 $cur_time= mktime(9, 00,00, date('m',time()),date('d',time()), date('Y',time()));    
  $update_to_be_procured = db_update('ebay_books_order_assigned');
  $update_to_be_procured->fields(array('status' => 'to_be_procured','dtime' => $cur_time));
  $update_to_be_procured->condition('status','freezed');
  $update_to_be_procured->execute(); 
  drupal_goto('sales/order/invoice/search');
}
/**
 * List all assign orders to the distributor
 * On calling sales/order/get_assignment/'.$distributor_id
 * 
 */
function sales_show_table_structure_assign_orders($distributor_id){
//   //  $query_check_active=db_select('ebay_books_order_assigned','oa2');
////     $query_check_active->fields('oa2',array('batch','status'));
////     $query_check_active->condition('oa2.status','procured');
////     $query_check_active->condition('oa2.status','assigned');
////     $query_check_active->condition('oa2.status','to_be_procured');
////     $query_check_active->addExpression('Distinct(oa2.batch)','batch_all');
//     $get_batch=db_select('ebay_books_order_assigned','oa');
//    $get_batch->addExpression('Distinct(oa.batch)','batch_all');
//    $result=$get_batch->execute()->fetchAll();
//   //  $query_check_active->execute()->fetchAll();
//    //print_r($result);die;
//    $co=1;
//   foreach($result as $c){
//    
//      $batch_ch=$c->batch_all;
//   //print_r($batch_ch);die;
//   $co++;
//   }
//if($co>1){
//     print_r($batch_ch);die;
//}
    
  $header = array('ISBN','Updated On(m-d-Y)','Supplier Assigned QTY', 'QTY in Stock','TITLE', 'PUBLISHER', 'REASSIGN TO', '');
  $data = sales_get_distributor_vise_orders($distributor_id);
  $rows = array();
  // Distributoe vise Information 
  foreach($data as $id => $book_info){
    $qty_av_isbn=sales_get_avl_qrt_by_isbn($book_info->isbn,$distributor_id);
    $last_upload_date=sales_get_latest_uploded_date($distributor_id,$book_info->isbn);
    $date_upload=date('m-d-Y',$last_upload_date);
     
    //if($book_info->isbn=='9788183554695'){ var_dump($last_upload_date);die;}
    if(empty($last_upload_date)){
        $date_upload="<b>Unavailable In Stock</b>";
        $qty_av_isbn="<b>NA</b>";
    }
    //print_r($last_upload_date);die;
    $args = '';
    $options = sales_get_reassign_options($book_info->isbn,$distributor_id,$book_info->qty,$book_info->price,$book_info->order_id);
    $order_assigned_id=$book_info->order_assigned_id;
    $row = array();
    $row[] = $book_info->isbn;
   //	$row[] = date('m-d-Y',$book_info->dtime);
    $row[] = $date_upload;
    $row[] = $book_info->qty;
    $row[] = $qty_av_isbn;    
    $row[] = isset($book_info->title) ? $book_info->title : 'NA';
    $row[] = isset($book_info->publisher) ? $book_info->publisher : 'NA';
    
   /* foreach($options as $key => $option){
        $args .= '<option value="'.$key.'" qty="'.$qty.'">'.$option.'</option>';
    }*/
    
    $row[] = '<select id = "select_'.$distributor_id.'_'.$book_info->isbn.'">' . $options . '</select>';
    $row[] = '<button id = "'. $distributor_id . "_" . $book_info->isbn .'_'.$order_assigned_id.'" class = "reassign"> Add </button>';
    $rows[] = $row;      
  }
  
  return '<div id = "distributor_'.$distributor_id.'">' . theme('table', array('header' => $header, 'rows' => $rows)) . '</div>';
 
}
function sales_get_latest_uploded_date($source,$isbn){
 
 
  $query=db_select('stock','s');
  if($source!=0){ 
  $query->condition('s.source',$source);
  }
  $query->condition('s.isbn13',$isbn);
  $query->addExpression('max(s.dtime)','dtime');
  $results = $query->execute()->fetchField();
  
  return $results;
   
  }
/**
 * UPDATE SLA ON DAILY BASIS
  
 */
function sales_update_sla(){
    //echo '<pre>';print_r($_SESSION); echo '</pre>';
    $update_sla=db_select('ebay_books_order_assigned','e');
    $update_sla->fields('e',array('isbn','dtime','temp_sla','order_id'));
    $update_sla->condition('e.temp_sla',0,'!=');
    $result_update_sla=$update_sla->execute()->fetchAll();
    $cur_time= mktime(9, 00,00, date('m',time()),date('d',time()), date('Y',time()));
    //echo date("ymd||H:i:s",$cur_time);die;1373513400;//
    foreach($result_update_sla as $up){
    $rec_dtime=$up->dtime;
    $diff_time_stamp=abs($cur_time-$rec_dtime);
    //echo($rec_dtime);die;
    //print_r($diff_time_stamp);die;
    $days_passed=round($diff_time_stamp/86400);
    //print_r($days_passed); die;
    if($days_passed!=0){
    $new_sla= (($up->temp_sla) - $days_passed);
    if($new_sla<0){
    $new_sla=0;
    }
    //print_r($new_sla);die;
    $update_sla = db_update('ebay_books_order_assigned');     
    $update_sla->fields(array('temp_sla' =>$new_sla));
    $update_sla->condition('isbn',$up->isbn);
    $update_sla->condition('order_id',$up->order_id);
    $update_sla->execute();
    $update_dtime = db_update('ebay_books_order_assigned');     
    $update_dtime->fields(array('dtime' =>$cur_time));
    $update_dtime->condition('isbn',$up->isbn);
    $update_sla->condition('order_id',$up->order_id);
    $update_dtime->execute();
    }
    }
    return true;
   }
   //emergency update not used regularly
function correct_sla(){
    $update_sla=db_select('ebay_books_order_assigned','e');
    $update_sla->fields('e',array('isbn','dtime','temp_sla'));
    $result_update_sla=$update_sla->execute()->fetchAll();
    foreach($result_update_sla as $up){
    $isbn=$up->isbn;
    $sla=calculate_sla($isbn);
    if($sla==''){$sla=5;}
    if(empty($sla)){$sla=5;}
    if($sla==NULL){$sla=5;}
    $update_sla = db_update('ebay_books_order_assigned');     
    $update_sla->fields(array('temp_sla' =>$sla));
    $update_sla->condition('isbn',$isbn);
    $update_sla->execute();
  }
 }
/**
 * Downlaod Invoice from Invoice Listing Page
 * From table :ebay_books_order_assigned
 */
function sales_invoice_download($distributor_id,$invoice_no){
	$header = array('PO ID','ISBN','Title','Author', 'Publisher','Language',
  'Stock Mrp','Catalog Mrp','Assigned QTY','Available Qty','Order Id','Date','Source','SLA');
	$inv_download = db_select('ebay_books_order_assigned','oa');
	$inv_download->fields('oa',array('isbn','order_assigned_id','qty','order_id','invoice_no','dtime','distrbutor_id','temp_sla'));//order_assigned_id
    $inv_download->condition('oa.status','assigned_waiting','!=');
    $inv_download->condition('oa.status','procured_waiting','!=');
    $inv_download->condition('oa.status','to_be_procured_waiting','!=');
    $inv_download->condition('oa.status','freezed_waiting','!=');
     if($distributor_id =='all' && $invoice_no =='all'){
    $inv_download->join('field_data_field_distributor_name' ,'dn','dn.entity_id = oa.distrbutor_id');
    $inv_download->fields('dn',array('field_distributor_name_value'));
    $inv_download->orderBy('oa.invoice_no','ASC');
    $inv_download->groupBy('oa.isbn,oa.distrbutor_id');
    $inv_download->addExpression('SUM(oa.qty)','sum_qty_all');
     	//$inv_download->groupBy('oa.distributor_id');
 	}else{
		$inv_download->condition('oa.invoice_no', $invoice_no); 
    $inv_download->groupBy('oa.isbn');
    $inv_download->addExpression('SUM(oa.qty)','sum_qty');
	}
	$date=date('m-d-Y',time());
	$results = $inv_download->execute()->fetchAll();
	$i=1;
    
 	foreach($results as $inv_down){	
		$inv_cat = db_select('catalog','c')
		->fields('c',array('title','author1','publisher','language','mrp_inr'))
		->condition('c.isbn13',$inv_down->isbn)
		->range(0,1)
		->execute()->fetchAll();		
		$inv_stk = db_select('stock','s')
		->fields('s',array('price'))
		->condition('s.isbn13',$inv_down->isbn)
		->range(0,1)
		->execute()->fetchAll();
		$inv_stk_qty = db_select('stock','s')
		->fields('s',array('qty'))
		->condition('s.isbn13',$inv_down->isbn)
		->condition('s.source',$inv_down->distrbutor_id)
		->range(0,1)
		->execute()->fetchAll();
    $row=array();
     if($distributor_id ='all' && $invoice_no =='all') {
		$my_qty=$inv_down->sum_qty_all;
	  }
    else{
      $my_qty=$inv_down->sum_qty;
      
    }
    $sla=calculate_sla($inv_down->isbn);
    ##sla update off###
    //$sla_upload=calculate_sla($inv_down->isbn);
//  //  echo($inv_down->isbn);
//   // print_r ($sla);die;
//    if($sla_upload==''){$sla_upload=5;}
//    $update_sla = db_update('ebay_books_order_assigned');     
//    $update_sla->condition('isbn',$inv_down->isbn);
//    $update_sla->condition('order_id',$inv_down->order_id);
//    $update_sla->fields(array('temp_sla' =>$sla_upload));
//    $update_sla->execute(); 
//    
    
    $row[]=$inv_down->invoice_no;
    $row[]=$inv_down->isbn;
    $row[]=str_replace("\r",'',str_replace("\n",'',$inv_cat[0]->title));
    $row[]=str_replace("\r",'',str_replace("\n",'',$inv_cat[0]->author1)); 
    $row[]=str_replace("\r",'',str_replace("\n",'',$inv_cat[0]->publisher)); 
    $row[]=str_replace("\r",'',str_replace("\n",'',$inv_cat[0]->language)); 
    $row[]= $inv_stk[0]->price;
    $row[]= $inv_cat[0]->mrp_inr;
    $row[]=$my_qty;
    $row[]=$inv_stk_qty[0]->qty;
    $row[]=$inv_down->order_id;
    $row[]=date('m-d-Y',$inv_down->dtime);
     
     if($distributor_id =='all' && $invoice_no =='all') {
			$row[]=$inv_down->field_distributor_name_value;
            $row[]=$sla;
	  }
		if($i==1) {
			$qry = db_select('field_data_field_distributor_name','sn');
			$qry->fields('sn',array('field_distributor_name_value'));
			$qry->condition('sn.entity_id',$inv_down->distrbutor_id);	
			$qry->range(0,1);
			$sname = $qry->execute()->fetchField();
      if($sname=='' && $distributor_id !='all' ){
         $sname='Market';
         }
      if($sname=='' && $distributor_id =='all' ){
         $sname='ALL';
         }
				 
			if($distributor_id =='all'){ 
				$sname = '';
			}else{
				$sname = 'Source Name :- '.$sname.'--'.$inv_down->distrbutor_id;
			}			
		}
		$xls_content_row .= implode("\t", array_values($row)) . "\r\n";    
		$i++;
	}
	$xls_content_header = $sname."\n";
	$xls_content_header .= implode("\t", array_values($header));
	$xls_content = $xls_content_header."\n".$xls_content_row;
	$filename = 'PO-ID-NO-'.$invoice_no.'_'.date("d-m-Y"); 
	header("Content-type: text/plain; charset=UTF-8");
	header("Content-Disposition: attachment; filename=$filename");
	header("Content-Type: application/vnd.ms-excel"); 
	header("Pragma: no-cache");
	header("Expires: 0");
	print $xls_content;
	exit();   
	return TRUE;
}



function calculate_sla($isbn){//
     ////////////////////////sla/////////////////////////////////
    $query = db_select('stock','s');
    $query->join('ebay_books_order_assigned','sla','sla.isbn = s.isbn13'); 
    $query->condition('s.qty',10,'>=');
    $query->condition('sla.isbn', $isbn); 
    $query->fields('s',array('isbn13','source'));
    $result = $query->execute()->fetchAll();
    $newArray = array();
    foreach($result as $entity) {
    if(!isset($newArray[$entity->isbn13])){
    $newArray[$entity->isbn13] = array();
    }
    $newArray[$entity->isbn13][] = $entity->source;
    }
     
    $final_arr = array();
    foreach($newArray as $k=>$v) {
    if(is_array($v)) {       
    $nwqry =  db_select('field_data_field_sla','sla_ptr')
    ->fields('sla_ptr',array('field_sla_value'))
    ->condition('entity_id',$v,'IN');
    $nwqry->addExpression('MIN(field_sla_value)','minsla');
    $result = $nwqry->execute()->fetchAll(); 
    //print_r($result);die;
    $rows = array();
        $rows[]= $k;   
        $rows[]= $result[0]->minsla;   
        
    }
    } //print_r($result[0]->minsla);die;
    return $result[0]->minsla;  
 
}
function calculate_sla_process_sales($isbn){//
     ////////////////////////sla/////////////////////////////////
    $query = db_select('stock','s');
    $query->condition('s.qty',10,'>=');
    $query->condition('s.isbn13', $isbn); 
    $query->fields('s',array('isbn13','source'));
    $result = $query->execute()->fetchAll();
    $newArray = array();
    foreach($result as $entity) {
    if(!isset($newArray[$entity->isbn13])){
    $newArray[$entity->isbn13] = array();
    }
    $newArray[$entity->isbn13][] = $entity->source;
    }
     
    $final_arr = array();
    foreach($newArray as $k=>$v) {
    if(is_array($v)) {       
    $nwqry =  db_select('field_data_field_sla','sla_ptr')
    ->fields('sla_ptr',array('field_sla_value'))
    ->condition('entity_id',$v,'IN');
    $nwqry->addExpression('MIN(field_sla_value)','minsla');
    $result = $nwqry->execute()->fetchAll(); 
    //print_r($result);die;
    $rows = array();
        $rows[]= $k;   
        $rows[]= $result[0]->minsla;   
        
    }
    } //print_r($result[0]->minsla);die;
    return $result[0]->minsla;  
 
}
/**
 * Search results callback of /sales/invoice/search/result
 * From table :ebay_books_order_assigned
 */
  //POID ISBN TITLE AUTHOR PUBLISHER LANGUAGE PRICE QTY ORDERID DATE
 //Source ISBN TITLE LANGUAGE PUBLISHER
function sales_invoice_search_results($invoice_no){
   
    drupal_add_css(drupal_get_path('module', 'sales') . '/sales.css');
    $header = array('isbn'=>'ISBN', 'title'=>'Title','publisher'=>'Publisher','language'=>'Language',
    'field_distributor_name_value'=>'Source','qty'=>'Quantity', 'order_id'=>'Order Id', 'price'=>'Price' ,'sla'=>'SLA');
    $query = db_select('ebay_books_order_assigned','oa');//->extend('PagerDefault')->limit(10)
    $query->fields('oa',array('isbn','qty','price','order_id','order_assigned_id','invoice_no')); 
    $query->join('catalog','c','oa.isbn = c.isbn13');
    $query->fields('c',array('title','language','publisher'));
    $query->join('field_data_field_distributor_name' ,'dn','dn.entity_id = oa.distrbutor_id');
    $query->fields('dn',array('field_distributor_name_value'));
    $query->condition('oa.status','to_be_procured');
    $query->condition('oa.invoice_no',arg(4));
    $num_rows = $query->countQuery()->execute()->fetchField();
    $results = $query->execute()->fetchAll();
	//print_r($results);die;
//	echo round($num_rows/5); exit;
	
/*	foreach() {
	}*/
// print_r($num_rows);die;  
 if($num_rows>0){

	foreach($results as $val){
	   $sla=calculate_sla($val->isbn);
        $row = array();
        $row['isbn']= $val->isbn;
        $row['title']= $val->title;
        $row['publisher'] = $val->publisher;
        $row['language'] = $val->language;
        $row['field_distributor_name_value'] = $val->field_distributor_name_value;
        $row['language'] = $val->language;
        $row['qty'] = $val->qty;
        $row['order_id'] = $val->order_id;
        $row['price'] = $val->price;
        $row['sla']=$sla;
        
        $rows[$val->order_assigned_id] = $row;
        
  
        
	}
 //	print_r ($rows);die;
 $form['source_name']=array(
  '#type'=>'hidden',
  '#value'=>$val->field_distributor_name_value,
  );
	  $form['nextmain2']['back']=array(
    '#type' => 'markup',
    '#prefix'=>'<div id="nextmain2">',
    '#markup' =>l('BACK','sales/order/invoice/search'),
    '#suffix'=>'</div>',
   );
    $form['myselector'] = array (
		'#type' => 'tableselect',
		'#title' => 'My Selector',
		'#header' => $header,
		'#options' => $rows,
	);  
	  
	$form['flt_right'] = array(
	'#prefix' => '<div class="flR">',
	'#suffix' => '</div>',	
	);
   
	$form['flt_right']['procure'] = array (
		'#type' => 'submit',
    '#weight' => 1,
		'#value' => t('procure'),
	); 	
    
	$form['flt_right']['reassign'] = array (
		'#type' => 'submit',
    '#weight' => 2,
		'#value' => t('reassign'),
	); 
 
//	$form['proqw'] = array (
//		'#markup' => theme('pager'),	 
//	); 
 
	return $form;    
 }else{
     $form['nextmain2']['back']=array(
    '#type' => 'markup',
    '#prefix'=>'<div id="nextmain2">',
    '#markup' =>l('Back','sales/order/invoice/search'),
    '#suffix'=>'</div>',
    );
    $form['myselector'] = array (
    '#type' => 'tableselect',
    '#title' => 'My Selector',
    '#header' => $header,
    );
   
 drupal_set_message('No records in Freezed status for this purchase order    '.l('Search Again','/sales/order/invoice/search'));
 return $form;
 }
}

function bartik_progress_bar($variables) {
  $output = '<div id="progress" class="progress">';
  $output .= '<div class="bar"><div class="filled" style="width: ' . $variables['percent'] . '%"></div></div>';
  $output .= '<div class="percentage">' . $variables['percent'] . '%</div>';
  $output .= '<div class="message">' . $variables['message'] . '</div>';
  $output .= '</div>';

  return $output;
}

 function sales_invoice_process(){
    global $user; 
   $output .= drupal_render(drupal_get_form('sales_invoice_search_results'));
  //$output .= sales_invoice_search_results_function();//Get table output format with pagination.
    return $output;
 }

function sales_invoice_search_results_submit($form,&$form_state){
	$dum = array();
 
  if($form_state['values']['op'] == 'procure'){
    foreach($form_state['values']['myselector'] as $k=>$v) {
			if($v>0) {
				$dum[] = $v;
       // print_r($v);die;
			}
		}
     if(count($dum)>0) {
    $update_procured = db_update('ebay_books_order_assigned');     
    $update_procured->condition('invoice_no',trim(arg(4)));
    $update_procured->condition('order_assigned_id',$dum,'IN');
    $update_procured->fields(array('status' =>'procured'));
    $update_procured->execute(); 
		drupal_set_message('Selected ISBN of Source '.trim($form_state['values']['source_name']).' are updated with status procured');
   }} 
   else if($form_state['values']['op'] == 'reassign'){
		foreach($form_state['values']['myselector'] as $k=>$v) {
			if($v>0) {
				$dum[] = $v;
       
			}
		}
	 if(count($dum)>0) {
	    //print_r(count($v));die;	   	   
		$update_reassign = db_update('ebay_books_order_assigned');     
		$update_reassign->condition('invoice_no',trim(arg(4)));
		$update_reassign->condition('order_assigned_id',$dum,'IN');
		$update_reassign->fields(array('status' =>'assigned'));
		$update_reassign->execute();  
		drupal_set_message('Selected ISBN of Source '.trim($form_state['values']['source_name']).' are updated with status assigned');
	 }else {
		drupal_set_message('Please select atleast one item','error');
	 }	 
  }
}

/**
 * Fetch all records assigned to this distributor.
 * From table : sells_records_items, and : ebay_books_order_assigned
 */
function sales_get_distributor_vise_orders($distributor_id){
  $query = db_select('ebay_books_order_assigned','oa');
  //$query->leftjoin('sells_records_items','i',' i.custom_label = oa.isbn ');//JOIN isbn with temp table
  $query->fields('oa',array('isbn','order_assigned_id','qty','price','order_id'));//order_assigned_id
  
  $query->leftjoin('catalog', 'c', 'c.isbn13 = oa.isbn'); //JOIN isbm with catalog
 // $query->join('stock', 's', 's.isbn13 = oa.isbn');
 // $query->fields('s',array('dtime'));//dtime
  //$query->fields('i',array('custom_label','item_title'));//publisher
  $query->fields('c',array('publisher','title'));//publisher
  $query->condition('oa.distrbutor_id', $distributor_id); 
  $query->condition('oa.status','assigned'); 
  $query->addExpression('SUM(oa.qty)', 'qty');
  $query->groupby('oa.isbn');
  $results = $query->execute()->fetchAll();
  
  //$last_upload_date=sales_get_latest_uploded_date($distributor_id);
  //$results[0]->dtime = $last_upload_date;
  return $results;
}

/**
 * Implements callback of sales/order/reassign/ajax
 * Reasssigning the Book to another source.
 */ 
function sales_isbn_reassing(){
  $distributor = $_POST['distibutor'];//id of A
  $assign_to_B = $_POST['assignto'];// id of B
  $order_assigned_id= $_POST['assigned_order_id'];//auto increment id
  $required_quantity_A = $_POST['requirequantity'];//Quantity to be assigned by A to B
  $available_quantity_B = $_POST['availquantity'];//Quantity available with B
  $isbn = $_POST['isbn'];
  $price= $_POST['bookprice'];
  $order_id= $_POST['order_id'];
  //B has enough quantity to get assigned from A.
  if($required_quantity_A <= $available_quantity_B){
    
   sales_assign_isbn_new_source($distributor,$assign_to_B,$isbn,$required_quantity_A);
  }
  //B don't have enough quantity to give to A.
  elseif($available_quantity_B<$required_quantity_A){
    sales_update_existing_source_looping($distributor,$assign_to_B,$isbn,$required_quantity_A,$available_quantity_B);
  }
  echo sales_show_table_structure_assign_orders($distributor); die;
}

/**
 * Ftech all distributors Name for assiged orders
 */ 
function sales_get_all_order_assigned_tabs(){
  $query = db_select('ebay_books_order_assigned','s'); 
  $query->condition('s.status', 'assigned', '='); 
  $query->fields('s',array('distrbutor_id'));
  $query->groupBy('s.distrbutor_id');
  $assignedto = $query->execute()->fetchAll();  
  return $assignedto;
}

function sales_show_table_structure_assign_orders_submit($form,&$form_state){
    echo "()()";
    print_r($form_state['values']); die;
}

/**
 * Check for Source has already book?
 */
function sales_check_source_has_book($dist_id,$isbn13){
  $num_select =db_select('ebay_books_order_assigned','temp');
  $num_select->fields('temp',array('order_assigned_id','qty'));
  $num_select->condition('isbn',$isbn13,'='); 
  $num_select->condition('distrbutor_id',$dist_id,'=');
  $num_select->condition('status','assigned','=');
  $res1 = $num_select->countQuery()->execute()->fetchField();
  $res = $num_select->execute()->fetchAllKeyed();
  return $res;   
}

/**
 * Assigned book to new source
 */
function sales_assign_isbn_new_source($distributor,$source_B,$isbn,$required_quantity_A){
  $source_has_book = sales_check_source_has_book($source_B,$isbn);
  
  if(!empty($source_has_book)){
    //B has $isbn already assigned to him, Update A to B which has same ISBN\
    sales_update_source($distributor,$source_B,$isbn);
  }else{
    //B dosenot have any qty of $isbn , Update A to B which has same ISBN
    sales_update_source($distributor,$source_B,$isbn);
  }
  return true;
}

function sales_deduct_isbn_previous_source($distributor,$isbn,$required_quantity){
  //A has
  return true;
}

/**
 * Update source id having same book isbn for reassigned process.
 */ 
function sales_update_source($source_A,$source_B,$isbn){
  $source_updated = db_update('ebay_books_order_assigned');     
  $source_updated->condition('isbn',$isbn,'=');
  $source_updated->condition('distrbutor_id',$source_A,'=');
  $source_updated->condition('status','assigned','=');
  $source_updated->fields(array('distrbutor_id' => $source_B));
  $source_updated->execute(); 
}
function sales_update_source_from_A_to_B($order_assigned_id,$assign_to_B,$isbn){
  $source_updated_A_TO_B = db_update('ebay_books_order_assigned');     
  $source_updated_A_TO_B->condition('order_assigned_id',$order_assigned_id,'=');
  $source_updated_A_TO_B->condition('status', 'assigned','=');
  $source_updated_A_TO_B->fields(array('distrbutor_id' => $assign_to_B));
  $source_updated_A_TO_B->execute();
}
function sales_update_existing_source_looping($distributor,$assign_to_B,$isbn,$required_quantity_A,$available_quantity_B){
    
  $select_target_dist=db_select('ebay_books_order_assigned','s');
  $select_target_dist->fields('s',array('isbn','qty','order_assigned_id','order_id'));
  $select_target_dist->condition('isbn',$isbn,'=');
  $source_updated->condition('status','assigned','=');  
  $select_target_dist->condition('distrbutor_id',$distributor,'=');
  $result_set = $select_target_dist->execute()->fetchAll();
  
  $check_limit=0;
  $qty_to_assign=$available_quantity_B;
  foreach($result_set as $key => $chk){
    $check_limit= $chk->qty;
    $order_assigned_id=$chk->order_assigned_id;
    $curr_order_id=$chk->order_id;
    if($check_limit<=$qty_to_assign){
      sales_update_source_from_A_to_B($order_assigned_id,$assign_to_B,$chk->isbn);
      $qty_to_assign-=$check_limit;//this will deduct current quantity from available qty of B
    }else if($qty_to_assign!=0){
      sales_insert_new_qty_of_B_source($curr_order_id,$assign_to_B,$isbn,$qty_to_assign,$price);
      sales_update_new_qty_of_A_source($check_limit-$qty_to_assign,$order_assigned_id);
      $qty_to_assign-=$check_limit;
    }
  }
}  
function sales_update_new_qty_of_prev_source($new_qty,$order_assigned_id){
    $qty_updated = db_update('ebay_books_order_assigned');     
    $qty_updated->condition('order_assigned_id',$order_assigned_id,'=');
    $qty_updated->condition('status', 'assigned','=');
    $qty_updated->fields(array('qty' => $new_qty));
    $qty_updated->execute(); 
    }
  function sales_insert_new_qty_of_B_source($curr_order_id,$assign_to_B,$isbn,$new_qty,$price){
    $sla_calc= calculate_sla($isbn);
    $cur_time= mktime(9, 00,00, date('m',time()),date('d',time()), date('Y',time()));
    
    $qty_insert=db_insert('ebay_books_order_assigned')
    ->fields(array(
        'order_id' => $curr_order_id,
        'distrbutor_id' => $assign_to_B,
        'isbn' => $isbn,
        'qty'=>$new_qty,
        'price' => floatval($price),
        'status' => 'assigned',
        'invoice_no'=>'',
        'dtime'=>$cur_time,   
        'batch'=>'', 
        'temp_sla'=>$sla_calc,                    
    ));
     $res = $qty_insert->execute();
    }
function sales_update_new_qty_of_A_source($qty_to_assign,$order_assigned_id){
  $qty_update = db_update('ebay_books_order_assigned');     
  $qty_update->condition('order_assigned_id',$order_assigned_id,'=');
  $qty_update->condition('status', 'assigned','=');
  $qty_update->fields(array('qty' => $qty_to_assign));
  $qty_update->execute(); 
}

  function sales_current_assigned_download(){
    $header = array('PO ID','ISBN','Title','Author', 'Publisher','Language',
  'Price','QTY','Source');
	$inv_download = db_select('ebay_books_order_assigned','oa');
	$inv_download->leftjoin('catalog', 'c', 'c.isbn13 = oa.isbn');
	$inv_download->fields('oa',array('isbn','order_assigned_id','qty','price','order_id','invoice_no','dtime'));//order_assigned_id
	$inv_download->fields('c',array('title','author1','publisher','language'));
  $inv_download->join('field_data_field_distributor_name' ,'dn','dn.entity_id = oa.distrbutor_id');
  $inv_download->fields('dn',array('field_distributor_name_value'));
  $inv_download->orderBy('dn.field_distributor_name_value','ASC');
 
	$date=date('m-d-Y',time());
  //$inv_download->groupBy('oa.isbn')->groupBy('oa.distrbutor_id');
	$results = $inv_download->execute()->fetchAll();
	$i=1;
	foreach($results as $inv_down){
    $row=array();
    $row[]=$inv_down->invoice_no;
    $row[]=$inv_down->isbn;
    $row[]=$inv_down->title;
    $row[]=$inv_down->author1;
    $row[]=$inv_down->publisher;
    $row[]=$inv_down->language;
    $row[]=$inv_down->price;
    $row[]=$inv_down->qty;
    $row[]=$inv_down->field_distributor_name_value;
 
		if($i==1) {
			$qry = db_select('field_data_field_distributor_name','sn');
			$qry->fields('sn',array('field_distributor_name_value'));
			$qry->condition('sn.entity_id',$distributor_id);	
			$qry->range(0,1);
			$sname = $qry->execute()->fetchField();
   	}
		$xls_content_row .= implode("\t", array_values($row)) . "\r\n";    
		$i++;
	}
 
	$xls_content_header .= implode("\t", array_values($header));
	$xls_content = $xls_content_header."\n".$xls_content_row;
	$filename = 'Current_auto_assignment'.$invoice_no.'_'.date("d-m-Y"); 
	header("Content-type: text/plain; charset=UTF-8");
	header("Content-Disposition: attachment; filename=$filename");
	header("Content-Type: application/vnd.ms-excel"); 
	header("Pragma: no-cache");
	header("Expires: 0");
	print $xls_content;
	exit();   
	return TRUE;
    
  }
function sales_get_avl_qrt_by_isbn($isbn,$distributor_id){
  $query=db_select('stock','s');
  $query->condition('s.isbn13',$isbn);
  $query->condition('s.source',$distributor_id);
  $query->fields('s',array('qty'));
  $results = $query->execute()->fetchField();
 if(empty($results)){$results=0;}
   //print_r($results);die;
   return $results;
  
 }