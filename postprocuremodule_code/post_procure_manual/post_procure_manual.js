(function($) {
   // (document.body).on('keypress',function(e)
$(document).ready(function(){ 
   //$(document).keypress(function (e) {
	 $('.procure_print_val').blur(function(e){
	  //(document.body).on('keypress',function(e){
	 // $("procure_print_val").bind('paste', function(e) {
	  // $(".procure_print_val").live('paste', function(e) {
	    // e.preventDefault();
         var bs = window.location.pathname.split('/');
         var isbn = $('.procure_print_val').val();
         var ofs = $('.procure_print_val').offset();
        
		if(isbn.length>0) {   
	 
            $(".procure_print_val").attr("disabled", "disabled"); 
                $.ajax({
                url : '/'+bs[1]+'/procuredmanual/pack/'+isbn,
              
                    success : function(data) {
                        var substr = data.split(':');
                       var check_complete = (substr[0]);
                       var order_id= substr[1];
                        var isbn2= substr[2];
                        var sla=substr[3];
                      if(check_complete=='ajax'){
                          
                        var prt = window.open('/'+bs[1]+'/procuredmanual/packprint/'+isbn2+'/'+order_id+'/'+sla,'Print invoice','left=20,right=20,width=900,height=500,resizable=1,scrollbars=1');
                   setTimeout(function(){prt.print();},2000);
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
                        // $('.popup_order').show('slow','swing');
                   
                       if(data=='No Records Found!!' || data==''){
                                
                           $('.popup_order').append('No Records Found!!!!');  
                        }
                        else{
                            $('.popup_order').append(data);
                        }
                     
                          
                        $('.popup_order #ok').click(function(e){
                        //$('.popup_order').slideUp();
                           
                        $(".procure_print_val").val() = '';
                        $(".procure_print_val").removeAttr("disabled");
                      
                       // e.preventDefault();
                        });

                      }
                      }
               
                        
                     }); 
                       
  
          
 
		}
	});
});
})(jQuery);