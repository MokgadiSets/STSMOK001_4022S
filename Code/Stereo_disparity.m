clear all;
cameraL = cv.VideoCapture(1);
cameraR = cv.VideoCapture(2);
detector = cv.FeatureDetector('SURF');
extractor = cv.DescriptorExtractor('SURF');
pause(2);
if ~cameraL.isOpened()
    error('left camera failed to be initialized');
end
if ~cameraR.isOpened()
    error('right camera failed to be initialized');
end
for i = 1:500
    left = cameraL.read();
    right = cameraR.read();
%%%%%%%%%%%%%%%%%%initializations%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

 

%bm = cv.StereoSGBM('MinDisparity',0);
%bm.BlockSize = 1;
%disparity2 = bm.compute(LeftRect,RightRect);

%%%%%%%%%%%%%%%%%keypoints and descriptors%%%%%%%%%%%%%%%%%%%%%%%%%%
%Left
keypointsL = detector.detect(left); 
keypointsL = cv.KeyPointsFilter.retainBest(keypointsL, 400);
descriptorsL = extractor.compute(left, keypointsL);

%Right
keypointsR = detector.detect(right);
keypointsR = cv.KeyPointsFilter.retainBest(keypointsR, 400);
descriptorsR = extractor.compute(right, keypointsR);


%%%%%%%%%%%%%%%%%Keypoint Matching%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
if ~isempty(keypointsL) && ~isempty(keypointsR)
    index_pairs = matchFeatures(descriptorsL, descriptorsR,'MaxRatio',0.4,'Unique',true); %indices of matching points
       
matchpointsL = keypointsL(index_pairs(:,1)); %To store the coordinates of matching points in left image
matchpointsR = keypointsR(index_pairs(:,2)); %To store the coordinates of matching points in right image
pointsL = [];
pointsR = [];
for e = 1:length(matchpointsL)
    pointsL = [pointsL ; matchpointsL(e).pt];
    pointsR = [pointsR ; matchpointsR(e).pt];
end
% figure; showMatchedFeatures(left,right,pointsL,pointsR);
% for k = 1:length(matchpointsL)
%     disp(k);
%     disp(matchpointsL(k));
%     disp(matchpointsR(k));
% end
% %%%%%%%%%%%%%%%%segmentation of points from keypoints%%%%%%%%%%%%%%%%%%
% points = [];
% for z =1: length(matchpointsR)
%         points = [points; matchpointsR(z).pt];
% end
% imshow(RightRect);
 hold on;

[C, ptsC, centres] = dbscan(transpose(pointsL), 40, 5); %adjust parameters depending on size of image
% 
% %%%%%%%%%%%%%%%%%%%Remove false positives%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
position = [];
distances = [];
widths = [];
heights = [];
for d = 1: max(ptsC)
    ToPlot = [];
    depth  = [];
    depth1 = [];
    step1 = 0.1;
    for c = length(matchpointsR):-1:1
        if (ptsC(c)<1) || (matchpointsL(c).response < 3500)
            matchpointsL(c) = [];
            matchpointsR(c) = [];
            ptsC(c) = [];
            
        elseif isequal(ptsC(c),d)
            ToPlot = [ToPlot; matchpointsR(c).pt];
%             dispValue = disparity2(round(matchpointsL(c).pt(2)),round(matchpointsL(c).pt(1)));
%             if (dispValue<1)
%                 dispValue = 1;
%             else
%                 depth1 = [depth1; ((790.74*89 )/dispValue)];
%             end
                depth = [depth; ((779*89 )/(matchpointsL(c).pt(1)-matchpointsR(c).pt(1)))];
        end
    end       
    
    if (~isempty(ToPlot))
        output = cv.boundingRect(ToPlot);
        %rectangle('Position', output, 'EdgeColor','b', 'LineWidth',1);
        %hold on;
        %str = sprintf(num2str(mode(depth)),5);
        %str1= sprintf(num2str(mode(depth1)),5);
        %annotation('textbox',[step1 step1 0.15 0.10],'string',str);
        position = [position; output];
        a = mode(depth);
        distances = [distances; a/1000];
        width = output(3)*(0.0011*(a/10) + 0.0086);
        height = output(4)*(0.0011*(a/10) + 0.0145);
        widths = [widths; width];
        heights = [heights;height];
        %disp(str);
        %disp(str1);
        %hold on;
    end
end
label_str = cell(length(widths),1);
for ii=1:length(widths)
    label_str{ii}=['Distance: ' num2str(distances(ii),'%0.2f') 'm' ' Size :' num2str(widths(ii),'%0.2f') 'x' num2str(heights(ii),'%0.2f') 'cm'];
end

if ~isempty(label_str)
    Display = insertObjectAnnotation(left,'rectangle',position,label_str,...
        'TextBoxOpacity',0.9, 'FontSize',18);

    imshow(Display),title('Distances');
else
    imshow(left),title('Distances');
end
end
    pause(0.1);
end