(function($) {
  $(document).ready(function(){
    $('.search_order_status').blur(function(){
        	var order = $('.search_order_status').val();
            var bs = window.location.pathname.split('/');
            if(order.length>0) {
                $.ajax({
                	url : '/'+bs[1]+'/sales/search/orderid/result/'+order,
                    success : function(data) {
                        if(data=='iksula'){
                           $('#sales_show_status').html('No Records Found');  
                        }
                        else{
                            $('#sales_show_status').html(data);
                        }
                        
                        
                    }
            });
            }
            
            
            
    });
    $('.abc').click(function(e){
        //alert($('.abc').attr('href'));
        var hre=$(this).attr('href');
        window.location.href = hre;
    });
   
    $('.next-popup').click(function(e){
      e.preventDefault();
      var ofs = $(this).offset();
      var value= $(this).html();
      var parts = value.split('_');
      var batch= parts[1];
      lihtml='/ebayconsole2/sales/dispose/batch/'+batch;
      //alert(lihtml);
      //$('.flR').html(lihtml);
      
      $('.abc').attr('href',lihtml);
      
      $(".next-popup-wrap").css('top', ofs.top-400);
      $(".next-popup-wrap").css('left', ofs.left-500);
      $('.next-popup-wrap').show('slow','swing');
      
    });
    
    $('.next-popup-wrap .action .flL').click(function(e){
      $('.next-popup-wrap').hide();
      e.preventDefault();
    });
    
 
     $('#page-title').val(' ');
     $('.reassign').live("click",function(){
            var id = $(this).attr('id').split('_');
            var assignto_info = $('#select_'+id[0]+'_'+id[1]).val();
            var clas = '#select_'+id[0]+'_'+id[1];
            
             var avlqty = $(clas +' option:selected').attr('avlqty');
             var reqqty = $(clas +' option:selected').attr('reqqty');
             var bkprice = $(clas +' option:selected').attr('bkprice');
             var distrb_b = $(clas +' option:selected').attr('distr_b');
             var order_id = $(clas +' option:selected').attr('order_id');
          if(assignto_info != 0){
            //alert(avlqty+'<'+reqqty);
            if(avlqty - reqqty <0){
              
              var r=confirm("Are you sure you want to do partial reassign?");
              
              
              if (r==true){
                                
                $.post("/ebayconsole2/sales/order/reassign/ajax",{distibutor: id[0], isbn: id[1], assignto : assignto_info,assigned_order_id : id[2],order_id : id[3],availquantity : avlqty,requirequantity : reqqty,bookprice : bkprice,distrb_b : distrb_b,order_id : order_id},function(result){
                  $('#distributor_'+id[0]).replaceWith(result);
                  $('#page-title').html('Book reassigned successfully').css("color","green");
                  
                });                
              }
              else
              {
                return false;           
              }
              
            }else{
              var r=confirm("Are you sure you want to reassign?");
              
              
              if (r==true){
                                
                $.post("/ebayconsole2/sales/order/reassign/ajax",{distibutor: id[0], isbn: id[1], assignto : assignto_info,assigned_order_id : id[2],order_id : id[3],availquantity : avlqty,requirequantity : reqqty,bookprice : bkprice,distrb_b : distrb_b,order_id : order_id},function(result){
                  $('#distributor_'+id[0]).replaceWith(result);
                 // $('#page-title').html('Book reassigned successfully. Page is Refreshing ...').css("color","green");
                 //  window.location.reload();
                  
                });                
              }
              else
              {
                return false;           
              }
            }
          }else{
            
            alert('Please select the reassign option.');
                return false;           
          }
      }); 
			
			$('#sales-order-assignment').submit(function(e){
				e.preventDefault();
				$('#sales_order_process_result').html('Loading please wait.....');
			});
			
  });
})(jQuery);