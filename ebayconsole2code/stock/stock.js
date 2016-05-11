(function($) {
$(document).ready(function(){    
    
   /* $('.stock_search_downlaod_file').show();
    $('#edit-stock-search-submit').click(function(){
	   $('.stock_search_downlaod_file').show();
	});*/
    
//	$('.page-stock-manage #edit-distributor-name').change(function(){
//	   $('#stock-manage-discount').submit();
//	});



 $("#edit-download-template").hide(); 
 $("#edit-discount-type-dld").click(function() {    
    $("#edit-download-template").hide();    
 });
 
 $("#edit-discount-type-dpd").click(function() {    
    $("#edit-download-template").show();    
 });
 
  $("#edit-discount-type-dpcd").click(function() {    
    $("#edit-download-template").show();    
 });



  
  $("#stock_excel_preview_area #submit").live("click", function() { 
	var isbn13 =-1;
	var qty = -1;
	var price = -1;
	var currency = -1;
	var disc = -1;
	var template_name = $('.page-create-stock-template #edit-template-name').val();
	var email = $('.page-create-stock-template #edit-email').val();
	var frequency = $('.page-create-stock-template #edit-frequency').val();
    var phone_no = $('.page-create-stock-template #edit-phone-no').val();
    var address = $('.page-create-stock-template #edit-address').val();
	$('.stock_mapping_select').each(function(){
		if($(this).val()){
			//alert($(this).val());
			if($(this).val() == 'isbn13'){
				var temp_id = $(this).attr('id').split('_');
				isbn13 = temp_id[1];
			}
			else if($(this).val() == 'qty'){
				var temp_id = $(this).attr('id').split('_');
				qty = temp_id[1];
			}
			else if($(this).val() == 'price'){
				var temp_id = $(this).attr('id').split('_');
				price = temp_id[1];
			}
			else if($(this).val() == 'currency'){
				var temp_id = $(this).attr('id').split('_');
				currency = temp_id[1];
			}
			else if($(this).val() == 'disc'){
				var temp_id = $(this).attr('id').split('_');
				disc = temp_id[1];
			}
		}
	});
	
	$.ajax({
		type: "POST",
		url: "stock_ajax_save_template",
		data: "template_name="+template_name+"&email="+email+"&frequency="+frequency+"&isbn13="+isbn13+"&qty="+qty+"&price="+price+"&currency="+currency+"&disc="+disc+"&phone_no="+phone_no+"&address="+address,
		success: function(msg){
			alert("Distributor saved succesfully");
			window.location.replace("create_stock_template");
		}	
 });
	return false; 
 });
    
    
    
    
    
});
})(jQuery);