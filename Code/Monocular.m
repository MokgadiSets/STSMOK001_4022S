% This is an object detector using one camera and an image stored in your file system
%For real time detector, see webcam.m

clear all;
close all;
a = imread('ocean1.jpg'); % put in any image from file


detector = cv.FeatureDetector('FastFeatureDetector');%,'MaxFeatures',75); %could use any other feature detector.

keypoints1 = detector.detect(a); %detect keypoints
keypoints = cv.KeyPointsFilter.retainBest(keypoints1, 400); %retain the best 400
 
points= [];   %array for storing pointd
imshow(a);    %plot image
hold on;
for z =1: length(keypoints)         %This loop is for plotting keypoints.
        %points = [points; keypoints(z).pt];
        plot(keypoints(z).pt(1),keypoints(z).pt(2) ,'x', 'LineWidth', 1, 'Color', 'red') 
end


[C, ptsC, centres] = dbscan(transpose(points), 30, 10); %form clusters
for d = 1: max(ptsC)
    ToPlot = [];
    for c = length(keypoints):-1:1   %This loop is for plotting bounding boxes
        if (ptsC(c)<1)    %remove points classified as noise
            keypoints(c) = [];
            ptsC(c) = [];
  
        elseif isequal(ptsC(c),d)
            ToPlot = [ToPlot; keypoints(c).pt];
        end
    end
   output = cv.boundingRect(ToPlot);
   hold on; rectangle('Position', output,'EdgeColor','r', 'LineWidth',1); % Plot each bounding rect
end


hold off;

 
