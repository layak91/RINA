//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package rina.examples._DublinBackup.UseCase5;


import ned.DelayChannel;
import rina.src.CS.BorderRouter2n1nm;
import rina.src.CS.Host1AP;
import rina.src.CS.InteriorRouter2Int;
import rina.src.CS.InteriorRouterNInt;

network UseCase5
{
    
    submodules:
        BorderRouterA: BorderRouter2n1nm {
            @display("p=147,145");
        }
        BorderRouterB: BorderRouter2n1nm {
            @display("p=379,145");
        }
        HostA: Host1AP {
            @display("p=41,50");
        }
        HostB: Host1AP {
            @display("p=482,50");
        }
        InteriorRouter: InteriorRouter2Int {
            @display("p=263,145");
        }
    connections:
        HostA.medium <--> DelayChannel {  delay = 1ms; } <--> BorderRouterA.medium[0];
        HostB.medium <--> DelayChannel {  delay = 1ms; } <--> BorderRouterB.medium[0];
        BorderRouterA.medium[1] <--> DelayChannel {  delay = 1ms; } <--> InteriorRouter.medium++;
        BorderRouterB.medium[1] <--> DelayChannel {  delay = 1ms; } <--> InteriorRouter.medium++;
}
