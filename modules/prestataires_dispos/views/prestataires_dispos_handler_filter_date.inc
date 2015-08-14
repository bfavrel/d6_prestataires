<?php

class prestataires_dispos_handler_filter_date extends views_handler_filter_date {
    
    function op_between($field) {
        
        if ($this->operator == 'between') {
            $a = intval(strtotime($this->value['min'], 0));
            $b = intval(strtotime($this->value['max'], 0));
        }
        else {
            $a = intval(strtotime($this->value['max'], 0));
            $b = intval(strtotime($this->value['min'], 0));
        }

        if ($this->value['type'] == 'offset') {
            $a = (integer)time() + (integer)sprintf('%+d', $a); // keep sign
            $b = (integer)time() + (integer)sprintf('%+d', $b); // keep sign
        }
        
        // %s is safe here because strtotime scrubbed the input and we might
        // have a string if using offset.
        $this->query->add_where($this->options['group'], "$field >= '%s'", date("Y-m-d", $a));
        $this->query->add_where($this->options['group'], "$field <= '%s'", date("Y-m-d", $b));
    }

    function op_simple($field) {
        
        $value = intval(strtotime($this->value['value'], 0));
        
        if (!empty($this->value['type']) && $this->value['type'] == 'offset') {
            
            $this->query->add_where($this->options['group'], 
                "%s %s DATE_ADD('***CURRENT_DATE***', INTERVAL %d SECOND)",
                $field, $this->operator, $value);
        }
        else {
            $this->query->add_where($this->options['group'], "$field $this->operator %s", $value);
        }
    }
}