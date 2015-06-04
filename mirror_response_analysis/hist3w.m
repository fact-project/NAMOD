function[hist_map]=hist3w(data,x_grid,y_grid)


 
hist_map = zeros(length(x_grid),length(y_grid));

for t=1:1:size(data,1)
    X=-1;
    Y=-1;
    % locate x
    for x=1:1:length(x_grid)-1
        if( x_grid(x)<data(t,1) && x_grid(x+1)>=data(t,1))
            X = x;
        end
    end

    % locate y
    for y=1:1:length(y_grid)-1
        if( y_grid(y)<data(t,2) && y_grid(y+1)>=data(t,2))
            Y = y;
        end
    end    
    
    if(X==-1 || Y==-1)
        disp('Out of range')
    else
        hist_map(X,Y)=hist_map(X,Y)+data(t,3);
    end

end


end