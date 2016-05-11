<?php
else if ($_GET['op'] == 'Stock Download Unmatched Price')  //download records where price difference between stock and catalog  is greater than 30%
   {
  // $subquery = db_select('field_data_field_distributor_name','ds');
  // $subquery->fields('ds',array('field_distributor_name_value'));
  //$subquery->condition('ds.entity_id','s.source','=')
  
    $query = db_select('stock','s');
    $query->join('catalog','c','c.isbn13 = s.isbn13');
    $query->join('field_data_field_distributor_name','ds','ds.entity_id = s.source');
    //$query->join('field_data_field_value','fv','fv.entity_id = s.currency');
    $query->groupBy('s.isbn13');
    $query->fields('s',array('isbn13','source','dtime','price','qty','currency'));
    $query->fields('ds',array('field_distributor_name_value'));
   // $query->fields('fv',array('field_value_value'));
    $query->fields('c',array('mrp_inr'));
    $query->addExpression('MAX(s.price)', 'max_price_stock');
    $query->addExpression('MAX(c.mrp_inr)', 'max_price_cata');
    $query->addExpression('SUM(s.qty)', 'sum_qty_stock');
  //  $query->condition('isbn13',9780006165736);
   // $query->range(0,100);
   
    if($template != 0){
    	 $query->condition('s.source',$template);
}
    $result = $query->execute()->fetchAll();
    $header = array('ISBN13','Stock_MRP','Catalog_MRP','Source','MRP_Difference','Last_Arrived','Quantity');
    foreach($result as $filter_val){
        $subquery1=db_select('stock','ss');
        $subquery1->fields('ss',array('currency'));
        $subquery1->condition('ss.isbn13',$filter_val->isbn13);
       // $subquery1->condition('ss.source',$filter_val->source);
        $subquery1->condition('ss.price',$filter_val->max_price_stock);
        $subquery1->range(0,1);
        $cur_orig= $subquery1->execute()->fetchField();
      //  print_r($cur_orig);die;
        $subquery2=db_select('field_data_field_value','fv');
        $subquery2->fields('fv',array('field_value_value'));
        $subquery2->condition('fv.entity_id',$cur_orig,'=');
        $subquery2->range(0,1);
        $cur= $subquery2->execute()->fetchField();
        
        $compare_price=0;
        $currency=$filter_val->currency;
      //  $price_as_currency=$filter_val->field_value_value;
        $max_stock = $filter_val->max_price_stock;
         
          if($currency!=1 && $currency!=3 && $currency!=1048 && $currency!=1056 && $currency!=1070 &&
           $currency!=1085 && $currency!=1104 && $currency!=1111 && $currency!=1131 && $currency!=1184){
            //1184 1131 1111 1104 1085 1070 1056 1048 3 1 rupees :()
              $converted_max_stock = $max_stock * $cur;
              $max_stock=$converted_max_stock;
          }
        
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
   else{echo $Range_Price;die;} 
  if($from && $to) {
    if($compare_price==1 && $filter_val->dtime > $from && $filter_val->dtime < $to){//&& $filter_val->dtime > $from && $filter_val->dtime < $to //$filter_val->max_qty >10  &&
        $rows = array();
        $rows[]= $filter_val->isbn13; 
        $rows[]= $max_stock;  
        $rows[]=$filter_val->max_price_cata;
        $rows[]=$filter_val->field_distributor_name_value;
        $rows[]=$per_diff_price.'%';
        $rows[]=$date_dtime;
        $rows[]=$filter_val->sum_qty_stock;
        $xls_content_row .= implode("\t", array_values($rows)) . "\r\n";
    }
    }
    else{
      if($compare_price==1){//&& $filter_val->dtime > $from && $filter_val->dtime < $to //$filter_val->max_qty >10  &&
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

?>