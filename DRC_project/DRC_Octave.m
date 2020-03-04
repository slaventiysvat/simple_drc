clear 
clc

%choosing method of estimating envelope
PEAK_DET = 1;

RMS_DET = 0;%works incorect in Octave
%--------------------------------------
NT = 8;
LT = 90;
EX = 10;
CT = 45;

CT_N =  0.01 * NT;
CT_LT = 0.01 * LT;
CT_EX = 0.01 * EX;
CT_CT = 0.01 * CT;

gain = 200;

threshoultLT = CT_LT;
threshoultNT = CT_N;
threshoultEX = CT_EX;
threshoultCT = CT_CT;

alfaNG = (0* pi) / 180;%1
alfaEX = (1* pi) / 180;
alfaCM = (150* pi) / 180;
alfaLT = (90* pi) / 180;

y = zeros(1,101);
x = 0:0.01:1;
y = x;
%----------------------------static curve---------------------------------------
%static curve for example
if (alfaNG > 0)
  
k = 1/tan(alfaNG);

else

k = 0;

end
%noise gate
for i=1:1:101
  
  %noise gate
  if (y(i) < CT_N)
    
    k = 1/tan(alfaNG);
    
    y(i) = CT_N + (k*(x(i) - CT_N));
    
    if (y(i) < 0)
       
         y(i) = 0;
       
    end
    
 %expander
 elseif ((y(i) > CT_N) && (y(i) <= CT_EX))
  
   y(i) = CT_N + (1/tan(alfaEX))*(x(i) - CT_N);


 %straight curve
  elseif  ((y(i) > CT_EX) && (y(i) <= CT_CT))

     y(i) = x(i) + gain * 0.01;

 %compressor
  elseif ((y(i) > CT_CT) && (y(i) <= CT_LT))
   
  y(i) = CT_LT + (1/tan(alfaCM))*(x(i) - CT_LT);
  
 %Limiter
 elseif ((y(i) > CT_LT))
 
  y(i) = CT_LT + (1/tan(alfaLT))*(x(i) - CT_LT);
 
 end
 
 
end


figure
x1 = 0:0.01:1;
plot(x1)
hold on
plot(y)
ylim([0 1])
legend('simple curve','modified curve');

[in,Fs] = audioread('..\DRC_project\check2.wav');
S = in;
if (PEAK_DET == 1)
%-------------------------------peak detector-----------------------------------

    attack_time = 0.2;%
    release_time = 100.0;%
    AT = (-2.2 *(1.0 / Fs)) / (attack_time / 1000.0);
    RT = (-2.2 *(1.0 / Fs)) / (release_time / 1000.0);
    AT = 1.0 - exp(AT);
    RT = 1.0 - exp(RT);
    previous_y = 0;%initial value
    in = S;
    out = zeros(1,length(in));

    if (abs(in(1)) > previous_y) 

      out(1) = (1 - AT) * previous_y + abs(in(1))*AT;

    end

    if (abs(in(1)) <= previous_y) 

      out(1) = (1 - RT) * previous_y;
        
    end

    for i = 2:1:length(in)

           
        if (abs(in(i)) > out(i - 1)) 

          out(i) = (1 - AT) * out(i - 1) + abs(in(i))*AT;

            end
            
        if (abs(in(i)) <= out(i - 1)) 

          out(i) = (1 - RT) * out(i - 1);

         end

end  

   % out = out./max(out);
   % out = medfilt1(out,80);
   
    figure
    plot(S);
    hold on
    plot(out,'r','LineWidth',2)
    legend('Original Signal','Envelope')
    
end


%y = medfilt1(x,10);



if (RMS_DET == 1)
%-------------------------------RMS detector------------------------------------
      previous_y  = 0;

      %RMS_detector
      out = zeros(1,length(in));

      N = 100;

      out(1) = (previous_y * ((N - 1.0) / N)) + ((in(1)^2) * (1.0 / N));

      for i = 2:1:length(in)
       
        out(i) = (out(i - 1) * ((N - 1.0) / N)) + ((in(i)^2) * (1.0 / N));

      end

      for i = 1:1:length(in)

          out(i) = sqrt(out(i));

      end

      figure
      plot(in)
      hold on
      plot(out,'r')
      hold off
      title('RMS detector')
      legend('Original Signal','Envelope')
end


%-------------------Noise gate/expander/compressor------------------------------

new_dble = zeros(1,length(in));

for i = 1:1:length(in)
  
  %noise gate
  if (out(i) <= threshoultNT)
    
    new_dble(i) = 0;
    
  %expander
  elseif((out(i) > threshoultNT) && (out(i) <= threshoultEX))
  
    new_dble(i) =  CT_N + (1/tan(alfaEX))*(out(i) - CT_N);

  %straight curve * gain
  elseif ((out(i) > (threshoultEX + 0.01)) && (out(i) <= threshoultCT))
  
    new_dble(i) =  (0.01 * gain) + out(i);
    
  %compressor
  elseif ((out(i) > (threshoultCT+ 0.01)) && (out(i) <= threshoultLT))
  
   new_dble(i) = CT_LT + (1/tan(alfaCM))*(out(i) - CT_LT);
   
  %Limiter
  elseif (out(i) > threshoultLT)
   
     new_dble(i) = CT_LT + (1/tan(alfaLT))*(out(i) - CT_LT);
     
  end
  
 
end

new = zeros(1,length(in));

for i = 2:1:length(in)
    
    new(i) = new_dble(i)*in(i - 1);

end

%   %filtration with smoothie filter
%    attack_time = 0.2;%
%    release_time = 200;%
%    AT = (-2.2 *(1.0 / Fs)) / (attack_time / 1000.0);
%    RT = (-2.2 *(1.0 / Fs)) / (release_time / 1000.0);
%    AT = 1.0 - exp(AT);
%    RT = 1.0 - exp(RT);
%    previous_y = 0;%initial value
%    in = S;
%    out1 = zeros(1,length(in));
%
%    if (abs(new(1)) > previous_y) 
%
%      out1(1) = (1 - AT) * previous_y + abs(new(1))*AT;
%
%    end
%
%    if (abs(new(1)) <= previous_y) 
%
%      out1(1) = (1 - RT) * previous_y;
%        
%    end
%
%    for i = 2:1:length(new)
%
%           
%        if (abs(new(i)) > out1(i - 1)) 
%
%          out1(i) = (1 - AT) * out1(i - 1) + abs(new(i))*AT;
%
%            end
%            
%        if (abs(new(i)) <= out1(i - 1)) 
%
%          out1(i) = (1 - RT) * out1(i - 1);
%
%         end
%
%end  
%
%new1 = zeros(1,length(in));
%
%for i = 2:1:length(in)
%    
%    new1(i) = out1(i)*new(i - 1);
%
%end

figure
subplot(211)
plot(in,'b')
hold on
plot(out,'r')
legend('Original Signal','PEAK');
title('Signal with PEAK envelope');
grid on
hold off
subplot(212)
plot(out)
hold on
plot(new);
legend('PEAK','Original signal after');
grid on
hold off

%audiowrite('D:\DRC\DRC_project\DRC_project\merrychristmasdarlingMono_out.wav',new,Fs);
sound(new,Fs);

