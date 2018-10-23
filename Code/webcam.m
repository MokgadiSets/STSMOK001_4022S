clear all;
camera = cv.VideoCapture(0);
pause(2);
if ~camera.isOpened()
    error('camera failed to be initialized');
end
for i = 1:500
    frame = camera.read();
    detector = cv.FeatureDetector('SURF');
% tic
keypoints1 = detector.detect(frame);  
keypoints = cv.KeyPointsFilter.retainBest(keypoints1, 400);
% toc
points= [];
imshow(frame);

hold on;
for z =1: length(keypoints)
        points = [points; keypoints(z).pt];
end

[C, ptsC, centres] = dbscan(transpose(points), 70, 10);
for d = 1: max(ptsC)
    ToPlot = [];
    for c = length(keypoints):-1:1
        if (keypoints(c).response < 3500)||(ptsC(c)<1)
            keypoints(c) = [];
            ptsC(c) = [];
  
        elseif isequal(ptsC(c),d)
            ToPlot = [ToPlot; keypoints(c).pt];
        end
    end
    if (~isempty(ToPlot))
        output = cv.boundingRect(ToPlot);
        hold on; rectangle('Position', output,'EdgeColor','y', 'LineWidth',1);
    end
  
end

%
hold off;
    pause(0.1);
end
