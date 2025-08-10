classdef setup_lib

    methods(Static)

        % Following properties of 'maskInitContext' are available to use:
        %  - BlockHandle 
        %  - MaskObject 
        %  - MaskWorkspace: Use get/set APIs to work with mask workspace.
        function MaskInitialization(maskInitContext)
            maskWS = maskInitContext.MaskWorkspace;
        
            car_id = (maskWS.get('carID'));
            group_id = (maskWS.get('groupID'));
            broker = string(maskWS.get('brokerVal'));
            port   = string(maskWS.get('portVal'));
        
            % Les, men bruk berre viss dei ikkje er tomme
            userRaw = maskWS.get('userVal');
            passRaw = maskWS.get('passVal');
        
            hasUser = ~isempty(userRaw);

            % Vel riktig kall basert på kva som er tilgjengeleg
            if hasUser
                mqtt(car_id, group_id, broker, port, userRaw, passRaw);
            else
                mqtt(car_id, group_id, broker, port, "", "");

            end
        end
 

    end
end