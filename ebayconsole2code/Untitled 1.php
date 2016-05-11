<?php

/**
 * @author 
 * @copyright 2013
 */


/**
 * Implements helper function to calculate the price
 */
function stock_price_calculator($isbn,$mrp_inr = NULL ,$pub = NULL){
  if(!$mrp_inr){
    $query = db_select("catalog","c");
    $query->fields('c',array('mrp_inr','publisher'));
    $query->condition('c.isbn13',$isbn);
    $result = $query->execute()->fetchAll(); 
    $mrp_inr = $result[0]->mrp_inr;
    $pub = $result[0]->publisher;
  }
  
  $passon_discount = stock_get_passon_discount($isbn,$pub);
  $disbtr_dis = stock_get_disbtr_discount($isbn);
  $pubsrc_dis = stock_get_pub_src_discount($isbn,$pub);
  if($mrp_inr < 150){
    $passon_discount = 0;
  }else if($mrp_inr > 2000){
    $passon_discount = 10;
  }
  
  $discount_amount = ($mrp_inr *$passon_discount)/100;
  $listing_price = $mrp_inr - $discount_amount;
  return $listing_price;
}
?>