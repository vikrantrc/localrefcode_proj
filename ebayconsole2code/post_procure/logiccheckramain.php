<?php
Array ( [0] => stdClass Object ( [sales_record_number] => 57729 
[procurestatus] => 0 [quantity] => 1 [sell_rec_id] => 5432 [printed] => 0 )
 [1] => stdClass Object ( [sales_record_number] => 57729 [procurestatus] => 0 
 [quantity] => 1 [sell_rec_id] => 5433 [printed] => 0 ) )
 Array ( [0] => stdClass Object ( [sales_record_number] => 57729 )
  [1] => stdClass Object ( [sales_record_number] => 57762 )
   [2] => stdClass Object ( [sales_record_number] => 57763 )
    [3] => stdClass Object ( [sales_record_number] => 57869 ) 
    [4] => stdClass Object ( [sales_record_number] => 57926 )
     [5] => stdClass Object ( [sales_record_number] => 57945 )
      [6] => stdClass Object ( [sales_record_number] => 57980 )
       [7] => stdClass Object ( [sales_record_number] => 57991 ) )
         if(($print_counter==$num_rows && $print_score==1)||($print_score==1 && $op_mess_scan!=2 && $num_rows==1)){
            $check_how++;
            //print_r($rs_order_each->sales_record_number);die;
            if($print_score==1){$notified_sales_rec_no = $rs_order_each->sales_record_number;}
            if($notified_sales_rec_no != $sales_record_no){
            print_r("gggg");die;
            }
            //print_r($notified_sales_rec_no);die;
            $update_notify_status = db_update('sells_records_items')
            ->fields(array(
            'order_notify' => 1,
            
            ))
            ->condition('sales_record_number',$rs_order_each->sales_record_number,'=')
            ->execute();
            //print_r($notified_sales_rec_no);die;
            $output="Order no:<span style= 'color: #0071B3;'><b>".$notified_sales_rec_no."</b></span>:is ready to be shipped";
            $op_mess_scan=2;
            $rack_old=get_rack_by_order_id($notified_sales_rec_no);
            //print_r($rack_old);die;
            if($rack_old!="0" && ($qty_loop==1 || $qty_loop== $i)){ $output .= ":remove it from rack no =>".$rack_old;
            }
            // print_r($output);die;
            else{ $output .= ":No rack assigned to it";}
            // print_r($output);die;
            rack_full_check($notified_sales_rec_no);
            
            }
            
              if($print_score!=1 && $op_mess_scan!=2){
        // print_r($rs_order_each->sales_record_number);die; checkpoint1
        
     
      } 9780593072493
      Array ( [0] => stdClass Object ( [sales_record_number] => 57729 [procurestatus] => 1 [quantity] => 1 [sell_rec_id] => 5432 [printed] => 1 )
       [1] => stdClass Object ( [sales_record_number] => 57729 [procurestatus] => 0 [quantity] => 1 [sell_rec_id] => 5433 [printed] => 0 ) )

?>