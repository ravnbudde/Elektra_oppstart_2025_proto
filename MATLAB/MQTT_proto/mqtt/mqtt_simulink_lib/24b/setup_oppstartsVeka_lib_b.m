classdef setup_oppstartsVeka_lib

    methods(Static)

        % Following properties of 'maskInitContext' are available to use:
        %  - BlockHandle 
        %  - MaskObject 
        %  - MaskWorkspace: Use get/set APIs to work with mask workspace.
        function MaskInitialization(maskInitContext)
            maskWS = maskInitContext.MaskWorkspace;
        
            car_id = (maskWS.get('carID'));
            broker = '192.168.1.3';
            port   = '1883';
            user = 'BIAISbroker';
            pass = 'shinyteapot294';

            mqtt(car_id, broker, port, user, pass);
        end
 

    end
end