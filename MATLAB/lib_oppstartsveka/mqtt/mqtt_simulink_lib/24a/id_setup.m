classdef id_setup

    methods(Static)

        % Use the code browser on the left to add the callbacks.
        function MaskInitialization(maskInitContext)
            maskWS = maskInitContext.MaskWorkspace;
        
            car_id = (maskWS.get('carID'));
            group_id = (maskWS.get('groupID'));
            % Assert that id is not zero
            % assert(car_id ~= 0, 'Error: car_id cannot be zero');
            % assert(group_id ~= 0, 'Error: group_id cannot be zero');
            
            assignin('base', 'car_id', car_id);
            assignin('base', 'group_id', group_id);
        end
    end
end