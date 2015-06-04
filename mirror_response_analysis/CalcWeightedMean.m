function [M]=CalcWeightedMean(D)

M = zeros(2,1);
Weights = 0;
for i=1:1:size(D,1)
    M = M + D(i,3)*[D(i,1);D(i,2)];
    Weights = Weights + D(i,3);
end
M = M./Weights;

end