(function($) {
   // (document.body).on('keypress',function(e)
$(document).ready(function(){ 
     var pressed = false; 
     var chars = []; 
    $(document).keypress(function (e) {
	// $('.procure_print_val').blur(function(e){
	  // (document.body).on('keypress',function(e){
	  // $("procure_print_val").bind('paste', function(e) {
	  // $(".procure_print_val").live('paste', function(e) {
	    // e.preventDefault();
            //  if ((e.keyCode || e.which) == 13) {
        // Enter key pressed
        //location.reload();
    //}
     
        
         if (e.which >= 0) {
            // if a number is pressed we add it to the chars array
            chars.push(String.fromCharCode(e.which));
        }
          //console.log(e.which + ":" + chars.join("|"));
          
          
            if (pressed == false) {
            // we set a timeout function that expires after 1 sec, once it does it clears out a list 
            // of characters 
            setTimeout(function(){
                // check we have a long length e.g. it is a barcode
                if (chars.length >= 13) {
                    // join the chars array to make a string of the barcode scanned
                    var isbn = chars.join("");
                    // debug barcode to console (e.g. for use in Firebug)
                     //console.log("Barcode Scanned: " + isbn);
                    
                    // assign value to some input (or do whatever you want)
                     $(".procure_print_val").val(isbn);
                         
            
        
         var bs = window.location.pathname.split('/');
        // var isbn = $('.procure_print_val').val();
         var ofs = $('.procure_print_val').offset();
         // alert(isbn);
	    
  
            $(".procure_print_val").attr("disabled", "disabled"); 
                $.ajax({
                url : '/'+bs[1]+'/procured/pack/'+isbn,
              
                    success : function(data) {
                        var substr = data.split(':');
                       var check_complete = (substr[0]);
                       var order_id= substr[1];
                        var isbn2= substr[2];
                        var sla=substr[3];
                      if(check_complete=='ajax'){
                          
                        var prt = window.open('/'+bs[1]+'/procured/packprint/'+isbn2+'/'+order_id+'/'+sla,'Print invoice','left=20,right=20,width=900,height=500,resizable=1,scrollbars=1');
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
                chars = [];
                pressed = false;
            },500);
   
          pressed = true;
         }
	});
    
    $(".procure_print_val").keypress(function(e){
    if ( e.which === 13 ) {
        console.log("Prevent form submit.");
        e.preventDefault();
    }
});
});
})(jQuery);