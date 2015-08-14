<?php

class PrestatairesCalendar {

    public $dispos_by_day;
    public $dispos_datafields;

    public function PrestatairesCalendar() {
        $this->dispos_by_day = array();
    }

    public function create_month($year = 0,$month = 0, $classes) {

        $output = "";

        // Get today, reference day, first day and last day info
        if (($year == 0) || ($month == 0)){
            $referenceDay = getdate();
        } else {
            $referenceDay = getdate(mktime(0,0,0,$month,1,$year));
        }

        $firstDay = getdate(mktime(0,0,0,$referenceDay['mon'],1,$referenceDay['year']));
	$lastDay = getdate(mktime(0,0,0,$referenceDay['mon']+1,0,$referenceDay['year']));

	// Create a table with the necessary header informations
        $output .= '<div class="month_container ' . $classes . '">';
	$output .= '<table class="month">';
	$output .= '<tr><th colspan="7">' . t($referenceDay['month']) . " " . $referenceDay['year'] . "</th></tr>";
	$output .= '<tr class="days"><td>L</td><td>M</td><td>M</td><td>J</td><td>V</td><td>S</td><td>D</td></tr>';

	// Display the first calendar row with correct positioning
	$output .= '<tr>';

	if ($firstDay['wday'] == 0) $firstDay['wday'] = 7;

	for($i=1; $i < $firstDay['wday']; $i++) {
            $output .= '<td class="blank">&nbsp;</td>';
	}

	$actday = 0;

	for($i = $firstDay['wday']; $i <= 7; $i++) {
            $actday++;

            $id = '_' . $year . '-' . sprintf("%1$02d", $month) . '-' . sprintf("%1$02d", $actday);

            $class = $this->get_class_from_dispos_by_day($id);
            $html_data = $this->get_data_from_dispos_by_day($id);

            $output .= "<td id='$id'$class>$actday$html_data</td>";
	}

	$output .= '</tr>';

	//Get how many complete weeks are in the actual month
	$fullWeeks = floor(($lastDay['mday'] - $actday)/7);

	for ($i=0; $i < $fullWeeks; $i++) {
            $output .= '<tr>';

            for ($j=0;$j < 7;$j++) {
                $actday++;

                $id = '_' . $year . '-' . sprintf("%1$02d", $month) . '-' . sprintf("%1$02d", $actday);

                $class = $this->get_class_from_dispos_by_day($id);
                $html_data = $this->get_data_from_dispos_by_day($id);

                $output .= "<td id='$id'$class>$actday$html_data</td>";
            }

            $output .= '</tr>';
	}

	//Now display the rest of the month
	if ($actday < $lastDay['mday']) {
            $output .= '<tr>';

            for ($i=0; $i < 7; $i++) {
                $actday++;

                $id = '_' . $year . '-' . sprintf("%1$02d", $month) . '-' . sprintf("%1$02d", $actday);

                $class = $this->get_class_from_dispos_by_day($id);
                $html_data = $this->get_data_from_dispos_by_day($id);

                if ($actday <= $lastDay['mday']){
                    $output .= "<td id='$id'$class>$actday$html_data</td>";
                } else {
                    $output .= '<td class="blank">&nbsp;</td>';
                }
            }

            $output .= '</tr>';
	}

	$output .= '</table>';
        $output .= "</div>"; // .month_container

        return $output;
    }

    private function get_class_from_dispos_by_day($id) {

        $class = '';

        if(isset($this->dispos_by_day[$id]['type_dispo'])) {
            $class = " class='dispo_type_" . $this->dispos_by_day[$id]['type_dispo'] . "'";
        }

        return $class;
    }

    private function get_data_from_dispos_by_day($id) {

        $html_data = "";

        if(isset($this->dispos_by_day[$id]['dispo_data'])) {

            foreach($this->dispos_by_day[$id]['dispo_data'] as $key => $data) {
                if($data == '') {continue;}
                $html_data .= t($this->dispos_datafields[$key]) . ": <span class='dispo-field-" . $key . "'>" . $data . "</span><br />";
            }
        }
        
        $date = substr($id, 1, 10);
        $date = substr($date, 8, 2) . "/" . substr($date, 5, 2) . "/" . substr($date, 0, 4);

        if($html_data != "") {
            return "<div class='dispo_data' style='display: none;'><div class='dispo_data_date'>" . $date . "</div>" . $html_data . "</div>";
        } else {
            return "<div class='dispo_data' style='display: none;'><div class='dispo_data_date'>" . $date . "</div>" . t("No informations.") . "</div>";
        }
    }
}