clear all;
close all;
a = imread('ocean1.jpg');

detector = cv.FeatureDetector('KAZE');%,'MaxFeatures',75);
%detector = cv.FeatureDetector('FastFeatureDetector');%,'MaxFeatures',75);
 tic
keypoints1 = detector.detect(a); 
keypoints = cv.KeyPointsFilter.retainBest(keypoints1, 75);
 toc
points= [];
imshow(cv.putText(a,'',[50,50],'BottomLeftOrigin', false, 'FontScale',0.5, 'Color','r'));
hold on;
for z =1: length(keypoints)
        %points = [points; keypoints(z).pt];
        plot(keypoints(z).pt(1),keypoints(z).pt(2) ,'x', 'LineWidth', 1, 'Color', 'red')
end

%[labels, centers, compactness] = cv.kmeans(points,  2, 'Attempts', 4);
[C, ptsC, centres] = dbscan(transpose(points), 30, 10);
% for d = 1: max(ptsC)
%     ToPlot = [];
%     for c = length(keypoints):-1:1
%         if (ptsC(c)<1)%(keypoints(c).response < 3500)||(ptsC(c)<1)
%             keypoints(c) = [];
%             ptsC(c) = [];
%   
%         elseif isequal(ptsC(c),d)
%             ToPlot = [ToPlot; keypoints(c).pt];
%         end
%     end
%     output = cv.boundingRect(ToPlot);
%     hold on; rectangle('Position', output,'EdgeColor','r', 'LineWidth',1);
% end

%
hold off;
%toc
 
