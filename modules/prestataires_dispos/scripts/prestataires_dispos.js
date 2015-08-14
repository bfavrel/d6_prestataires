

Drupal.behaviors.prestataires_dispos_delete = function() {

    $('a.delete_dispo').unbind('click');

    $('a.delete_dispo').click(function(evt){
        evt.preventDefault();

        $(this).parent().children('input').val(1);
        $(this).parent().addClass('to_delete');
        $(this).remove();
    });
};

Drupal.behaviors.prestataires_dispos_calendar = function() {

    $('table.month td').unbind('click');

    $('table.month td').click(function(){

        if($(this).attr('id') == '') {return;}

        var dateFromId = $(this).attr('id').substring(1);

        if($('input#edit-start-date-value').val() == '') {

            $('input#edit-start-date-value').val(dateFromId);
            $('div#start_date_display').html(Drupal.t("Start date : ") + dateFromId);
            $('div#end_date_display').fadeIn('slow');
            $('input.valid_dispo').fadeIn('slow');
            $('.step_two').fadeIn('slow');

            firstClickedId = $(this).attr('id');
            $(this).addClass('selected_range_item');

            $(this).children('.dispo_data').children('span').each(function(index, element){
                $("input#edit-" + $(element).attr('class')).val($(element).text());              
            });

        } else if($('input#edit-end-date-value').val() == '') {
            $('input#edit-end-date-value').val(dateFromId);
            $('div#end_date_display').html(Drupal.t("End date : ") + dateFromId);
            $('.step_three').fadeIn('slow');

            lastClickedId = $(this).attr('id');
            $(this).addClass('selected_range_item');

            switchOutline = -1;

            $('table.month td').each(function(index, element){

                if($(element).attr('id') == null) {return;}

                if($(element).attr('id') == firstClickedId || $(element).attr('id') == lastClickedId) {
                    switchOutline *= -1;
                    return;
                }

                if(switchOutline == 1) {
                    $(this).addClass('selected_range_item');
                }
            });
        }
    });

    $('#edit-cancel').click(function(){
        $('table.month td').removeClass('selected_range_item');
    });

    $('table.month td').mouseover(function(){

        if($(this).attr('id') == '') {return;}

        var dispo_data_html = $(this).children('div.dispo_data').html()

        if(dispo_data_html != null) {
            $('div#dispos_data_display').html(dispo_data_html);
        }
    });

    $('table.month').mouseout(function(){
        $('div#dispos_data_display').html(Drupal.t("Roll over a date to see availabilities details."));
    });
};

