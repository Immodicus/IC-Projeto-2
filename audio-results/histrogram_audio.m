%----Original audio - 1st. order
figure(1)
[audioIn,fs] = audioread("../audio/sample04.wav");
entropy = spectralEntropy(audioIn,fs);
%plot(entropy);
spectralEntropy(audioIn,fs)
p = sum(audioIn)./length(audioIn);
e = -sum(p.*log2(p));


%----Audio decoding - 1st. order
figure(2)
[audioIn2,fs2] = audioread("../audio-results/sample04_dec_1er.wav");
entropy2 = spectralEntropy(audioIn2,fs2);
%plot(entropy2);
spectralEntropy(audioIn2,fs2)
p1 = sum(audioIn2)./length(audioIn2);
e1 = -sum(p1.*log2(p1));


