<?php

////////////////////////////////////////////////////////////
########################ZERO LOGIC########################################
    //for isbn which are not in todays input but will go out in output
     $start =  mktime(10, 00,00, date('m',time()),date('d',time()), date('Y',time()));
     $end =   mktime(22, 00,00, date('m',time()),date('d',time()), date('Y',time()));
    $query_zero=db_select('stock','ss');
    $query_zero->fields('ss',array('qty','source','dtime','currency','isbn13','price'));
    $query_zero->condition('ss.qty',0,'=');
    $query_zero->condition('ss.source',$nid);
    $query_zero->condition('ss.dtime', array($start,$end), 'BETWEEN');
    $query_zero->range(0,5);
    $result_zero=$query_zero->execute()->fetchAll();
   // print_r($result_zero);die;
    foreach($result_zero as $re){
        $price_zero=$re->price;
        $listing_price_zero = stock_price_calculator($re->isbn13);
        $cur_rate_zero=get_cur_rate($re->currency);
        $price_zero=$price_zero*$cur_rate_zero;
               if($price_zero==''){echo"dsdsdsds"; }
                 
         // print_r(($re->isbn13)."pri");die;
         if($price_zero !=0 && $listing_price_zero!=0 && $re->isbn13 != ''){
             //currency conversion
        $update_zero = db_update('stock');
		$update_zero->fields(array('price' => $price_zero));
        $update_zero->condition('isbn13',$re->isbn13);
        $update_zero->condition('qty',0,'=');
        $update_zero->condition('source',$nid);
        $update_zero->condition('dtime', array($start,$end), 'BETWEEN');
		$update_zero->execute(); 
       //discount application
        $qry = db_update('catalog');
        $qry->fields(array('listing_price' => $listing_price_zero));
        $qry->condition('isbn13',$re->isbn13);
        $qry->execute();
         }
       
  	
        
    }
     ################################################################################


?>