clc;
clear all;
close all;

%path = 'final_sccan\';
path = 'old_reflector\';
number_of_mirrors = 0;
global_mrm_location = zeros(2,1);
for i=1:1:50

    filename = ['mirror_ID',num2str(i),'_response_map.csv'];
    
    csv_file_dir = [path,filename];
    if(exist(csv_file_dir)==2)
       
        number_of_mirrors = number_of_mirrors + 1; 

        MRM{number_of_mirrors,1} = i;
        MRM{number_of_mirrors,2} = ...
        csvread(csv_file_dir,1,0);
        
        global_mrm_location = global_mrm_location + ...
        CalcWeightedMean(MRM{number_of_mirrors,2});
    end
    
    
end

global_mrm_location = global_mrm_location./number_of_mirrors;

%% global offset
if(1)
    global_mrm_location

    for t=1:1:number_of_mirrors
        MRM{t,2} = ...
            MRM{t,2} - ...
            repmat([global_mrm_location',0.0],size(MRM{t,2},1),1);
    end
end
%%
font_size  =12;
close all
clc
x_bins = 31;
y_bins = 31;

fov = 0.3; % in DEG

ctrs = cell(2,1);
ctrs{1} = linspace(-fov,fov,x_bins);
ctrs{2} = linspace(-fov,fov,y_bins);

overlay_mrm = zeros(x_bins,y_bins);
info = '';
for t=1:1:number_of_mirrors
% calculate mean of mrm
mean_mrm_location = CalcWeightedMean(MRM{t,2});

C=hist3w(MRM{t,2},ctrs{1},ctrs{2});
C = C*1e4;
total_response = sum(sum(C));
H = figure();
hold on
axis([-fov fov -fov fov])
axis square
h=pcolor(ctrs{1},ctrs{2},C);

plot(mean_mrm_location(2),mean_mrm_location(1),'x','linewidth',2)
text(mean_mrm_location(2)-.05,mean_mrm_location(1)-.05,...
    ['\color{blue}(',num2str(mean_mrm_location(2),2),'|',num2str(mean_mrm_location(1),2),')'],...
    'fontsize',font_size)
info = [info,10,num2str(MRM{t,1}),' R{?,MRD} = [',num2str(mean_mrm_location(2),2),' ',num2str(mean_mrm_location(1),2),'];'];

colormap(flipud(colormap(gray(max(max(C))))))
set(h,'edgecolor',[0.8,0.8,0.8]) 
colorbar()
title(['mirror ID ',num2str(MRM{t,1})],'fontsize',font_size,'interpreter','latex');
xlabel('x [DEG]','fontsize',font_size,'interpreter','latex');
ylabel('y [DEG]','fontsize',font_size,'interpreter','latex');
%grid on

set(gcf, 'PaperPosition', [-0.75 -1 20 18]); 
set(gcf, 'PaperSize', [ 18 16]); 
saveas(gcf,[path,'mirror_ID',num2str(MRM{t,1}),'_response_map'], 'pdf') 

overlay_mrm = overlay_mrm + C;

close(H)
end

%% overall psf before final sccan

total_response = sum(sum(overlay_mrm))

H = figure();
hold on
axis([-fov fov -fov fov])
axis square
h=pcolor(ctrs{1},ctrs{2},overlay_mrm);
colormap(flipud(colormap(gray(max(max(C))))))
set(h,'edgecolor',[0.8,0.8,0.8]) 
colorbar()
xlabel('x [DEG]','fontsize',font_size,'interpreter','latex');
ylabel('y [DEG]','fontsize',font_size,'interpreter','latex');
%grid on

set(gcf, 'PaperPosition', [-0.75 -1 20 18]); 
set(gcf, 'PaperSize', [ 18 16]); 
saveas(gcf,[path,'combined_mirror_response_map'], 'pdf') 


disp(info)










