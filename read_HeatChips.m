%This script is used to plot the temperature output from a heating
%system described in https://github.com/TAspert/ITO_heating_device.

% It reads the Arduino card on which has been uploaded the "HeatChips_Arduino_Program_withMatlabOutput.ino" program
% and plots the data into 3 graphs

%When pausing the matlab code, the arduino sends a constant PWM value to the
%mosfet equals to the mean of the last values

%Contact: theo.aspert@gmail.com

clear all

SerialPort='COM15'; %PICK THE SERIAL PORT HERE

%%Set up the serial port object
s = serialport(SerialPort,9600);
s.Timeout=5;
flush(s);
configureTerminator(s,"LF")
% s.FlowControl='none';
disp('Arduino connected')
%%
pause(1)
tic
i=1;
figure;
%%
% pid=fscanf(s,'%c');
% pid_value(:)=sscanf(pid,'%f,',3);
%freeze='0';
PID_froze='150';
while toc< 1500000
    %NB: we have to send string...there is a bug when sending/parsing? numbers.
    %writeline(s,freeze);
    writeline(s,PID_froze);
    %a=readline(s);
    b=readline(s);
    
    pause(9) %to replace with waitbuffersize
    temperature=readline(s);
    temperature_array(:,i)=sscanf(temperature,'%f,',3);
    
    %Compute a d
    if numel(temperature_array(3,:))>=12
        PID_froze=num2str(int16(mean(temperature_array(3,numel(temperature_array(3,:))-10:numel(temperature_array(3,:))))));
    end
    %PAUSE HERE IF NEED TO MANUALLY FREEZE AND SEND ARBITRARE VALUE OF
    %PID_FROZE
    time(i)=toc;

    subplot(3,1,1)
    plot(time(1:i)/60,temperature_array(1,1:i))
    hold on
    plot(time(1:i)/60,temperature_array(1,1:i))
    set(gca,'YLim',[25 31])
    xlabel('Time (min)')
    ylabel('Temperature of the chip(°C)')
    set(gca,'XLim',[0 time(i)/60])
    
    subplot(3,1,2)
    plot(time(1:i)/60,temperature_array(3,1:i))
    set(gca,'YLim',[-1 256])
    xlabel('Time (min)')
    ylabel('PID input (0-255)')
    set(gca,'XLim',[0 time(i)/60])
    
    subplot(3,1,3)
    plot(time(1:i)/60,temperature_array(2,1:i))
    set(gca,'YLim',[23 26])
    xlabel('Time (min)')
    ylabel('Temperature of the room(°C)')
    set(gca,'XLim',[0 time(i)/60])
    
    drawnow
    i=i+1;
end