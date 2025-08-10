classdef id_setup

    methods(Static)

        % Use the code browser on the left to add the callbacks.
        function MaskInitialization(maskInitContext)
            maskWS = maskInitContext.MaskWorkspace;
        
            car_id = (maskWS.get('carID'));
            group_id = (maskWS.get('groupID'));
            assignin('base', 'car_id', car_id);
            assignin('base', 'group_id', group_id);
        end
    end
end