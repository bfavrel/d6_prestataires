
Drupal.behaviors.prestataires_content_delete = function() {
    
    $('a.delete_content').unbind('click');
    
    $('a.delete_content').click(function(evt){
        evt.preventDefault();

        $(this).parent().children('input').val(1);
        $(this).parent().addClass('to_delete');        
        $(this).remove();
    });
}