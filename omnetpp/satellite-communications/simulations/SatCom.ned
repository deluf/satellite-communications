
package satellite_communications.simulations;

import satellite_communications.GroundStation;
import satellite_communications.Terminal;
import satellite_communications.Satellite;

network SatCom
{
    parameters:
        int N = default(10);
    submodules:
        groundStation: GroundStation {
            @display("p=89,175");
        }
        terminal[N]: Terminal {
            @display("p=347,175");
        }
        satellite: Satellite {
            @display("p=212,24");
        }
}

