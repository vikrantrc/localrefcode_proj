(function($) {
   // (document.body).on('keypress',function(e)
$(document).ready(function(){  
	$('.procure_print_val').blur(function(e){
	     e.preventDefault();
        
         
        var bs = window.location.pathname.split('/');
        var isbn = $('.procure_print_val').val();
        
        var qty = $('.procure_print_qty').val();
         if(qty==''){
        qty=1;
                   }
        var ofs = $('.procure_print_val').offset();
        
		if(isbn.length>0) {   
		  // $(".procure_print_val").val() = '';
            $(".procure_print_val").attr("disabled", "disabled"); 
                $.ajax({
                url : '/'+bs[1]+'/procured/pack/'+isbn+'/'+qty,
              
                    success : function(data) {
                        var substr = data.split(':');
                       var check_complete = (substr[0]);
                       var order_id= substr[1];
                        var isbn2= substr[2];
                        var sla=substr[3];
                      //  alert(isbn2); 
                      if(check_complete=='ajax'){
                          
                        var prt = window.open('/'+bs[1]+'/procured/packprint/'+isbn2+'/'+order_id+'/'+sla,'Print invoice','left=20,right=20,width=900,height=500,resizable=1,scrollbars=1');
               
                        

                         setTimeout(function(){prt.print();},6000);
                                    //prt.print();
                                    /*prt.close();*/
                                    /*window.onfocus=function(){ window.close();}*/
                        
                      // $("#tb").val("alpha");
                       $(".procure_print_val").removeAttr("disabled");
                       $(".procure_print_val").val('');
                  
                                              
                      } 
                     else{
                                              
                 	     $('.popup_order').css({'display':'block'});
                         $(".popup_order").css('top', ofs.top-400);
                         $(".popup_order").css('left', ofs.left-10);
                         $('.popup_order').show('slow','swing');
                   
                     
                        $(".popup_order").animate({
                        width: "70%",
                        height: "70%",
                        opacity: .8,
                        marginLeft: "0.6in",
                        fontSize: "2em",
                        borderWidth: "2px",
                        overflow:scroll
                        
                        },500 );
                   // $(".popup_order").animate({ scrollTop: 0 }, "slow");overflow:scroll
                        $('.popup_order').scroll(function(){
                        $("#ok")
                        .animate({"marginTop": ($('.popup_order').scrollTop() - 13) + "px"}, "fast" );
                          //$("#ok").animate({"fontSize": ($('#ok').css("font-size") + 2) + "px"}, "slow" );                        
                        });

                        
                        if(data=='No Records Found!!' || data==''){
                                
                           $('.popup_order').append('No Records Found!!!!');  
                        }
                        else{
                            $('.popup_order').append(data);
                        }
                        //$(".popup_order").scrollTop(300);
                          // $( ".popup_order" ).draggable();
                          
                        $('.popup_order #ok').click(function(e){
                        $('.popup_order').slideUp();
                        //( ".popup_order" ).hide( "drop", { direction: "down" }, "slow" );       
                        $(".procure_print_val").val() = '';
                        $(".procure_print_val").removeAttr("disabled");
                        //$(".popup_order").fadeOut(1600, "linear", complete);    
                        e.preventDefault();
                        });

                      }
                      }
               
                        
                     }); 
                       
  
          
 
		}
	});
});
})(jQuery);