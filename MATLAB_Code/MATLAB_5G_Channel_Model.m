function PathLoss=MATLAB_5G_Channel_Model(grid_length,step_length,rxheight,txs,filepath)

x=-0:step_length:grid_length;
y=-0:step_length:grid_length;
[X,Y]=meshgrid(x,y);
m=size(X,1);
Z=zeros(m,m);

rxpos=reshape(X,[],1)';
rypos=reshape(Y,[],1)';
rzpos=reshape(Z,[],1)';

%Create rxsites across the grid area
rxs = rxsite("AntennaPosition", [rxpos;rypos;rzpos],"CoordinateSystem",'cartesian',"AntennaHeight",rxheight)';

%calculate pathloss between tx rx at each x,y location
%pm = propagationModel('freespace');
%pm = propagationModel('gas','Temperature',50);
%pm = propagationModel('rain','RainRate',50);
pm = propagationModel('close-in','PathLossExponent',3.3);
%pm = propagationModel('freespace') + propagationModel('gas','Temperature',50) + propagationModel('rain','RainRate',50);  
PathLoss = pathloss(pm,rxs,txs)';

%Write pathloss input file
Output=[rxpos',rypos',PathLoss];
writematrix(Output,filepath)
end