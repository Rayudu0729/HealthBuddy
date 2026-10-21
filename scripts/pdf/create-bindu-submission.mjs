import fs from 'node:fs/promises';
import { PDFDocument, rgb, StandardFonts } from './minipdf.mjs';

import { fileURLToPath } from 'node:url';
const root=fileURLToPath(new URL('../../', import.meta.url)).replace(/\\/g, '/').replace(/\/$/, '');
const out=root+'/output/pdf/Bindu-Project.pdf';
const pdf=await PDFDocument.create();

const regular=await pdf.embedFont(StandardFonts.Helvetica);
const bold=await pdf.embedFont(StandardFonts.HelveticaBold);
pdf.setTitle('Bindu - Standalone Safety and Accessibility Wearable');
pdf.setAuthor('Bindu Project Team');
pdf.setSubject('HackWithAditya 2026 project submission - concept and prototype development');
pdf.setCreator('Bindu Project');
pdf.setCreationDate(new Date('2026-10-20T06:30:00Z'));
const W=595.28,H=841.89,M=44,CW=W-2*M;
const color=(s)=>rgb(parseInt(s.slice(0,2),16)/255,parseInt(s.slice(2,4),16)/255,parseInt(s.slice(4,6),16)/255);
const navy=color('12263A'), teal=color('116B63'), muted=color('536779'), light=color('EDF5F2'), line=color('D9E3E7'), amber=color('98611E'), ink=color('243746');
let page; const bounds=[];
function rect(x,t,w,h,fill,border){page.drawRectangle({x,y:H-t-h,width:w,height:h,color:fill,borderColor:border,borderWidth:border?0.6:0});}
function rule(t){page.drawLine({start:{x:M,y:H-t},end:{x:W-M,y:H-t},thickness:0.7,color:line});}
function text(s,x,t,size=10,font=regular,c=ink){page.drawText(s,{x,y:H-t-size,font,size,color:c});}
function para(s,x,t,w,size=10.5,leading=15,c=ink,font=regular){
 let y=t,words=s.split(/\s+/),row='';
 for(const word of words){const candidate=row?row+' '+word:word;if(font.widthOfTextAtSize(candidate,size)>w&&row){text(row,x,y,size,font,c);y+=leading;row=word;}else row=candidate;}
 if(row){text(row,x,y,size,font,c);y+=leading;}
 if(y>H-58)throw Error('Page overflow at '+s.slice(0,50)); bounds.push({page:pdf.getPageCount(),bottom:y,text:s.slice(0,50)});return y;
}
function label(s,x,t){text(s.toUpperCase(),x,t,8,bold,teal);}
function heading(s,t){text(s,M,t,17,bold,navy);}
function base(n,kicker,title,subtitle){page=pdf.addPage([W,H]);rect(0,0,W,7,teal);text('BINDU',M,24,11,bold,teal);text('HACKWITHADITYA 2026  /  HEALTHTECH',253,27,7.6,regular,muted);label(kicker,M,65);text(title,M,86,25,bold,navy);if(subtitle)para(subtitle,M,125,CW,10.5,15,muted);rule(H-49);text('Bindu  |  Project submission  |  20 October 2026',M,H-36,8,regular,muted);text(String(n).padStart(2,'0')+' / 04',W-M-36,H-36,8,bold,teal);}
function card(title,body,x,t,w,h){rect(x,t,w,h,light);label(title,x+13,t+12);const y=para(body,x+13,t+34,w-26,10,14);if(y>t+h-8)throw Error('Card overflow '+title);}
function bullet(title,body,t){text(title,M,t,11,bold,navy);return para(body,M,t+20,CW,10.4,15)+15;}
function row(cols,t,widths,height=48,header=false){let x=M;cols.forEach((v,i)=>{rect(x,t,widths[i],height,header?navy:(Math.round(t)%2?color('F6F9FA'):color('FFFFFF')),line);const y=para(v,x+10,t+10,widths[i]-20,header?9:9.3,13,header?color('FFFFFF'):ink,header?bold:regular);if(y>t+height-5)throw Error('Table overflow: '+v);x+=widths[i];});}

// Page 1: executive summary and abstract.
base(1,'Project overview','Safety and accessibility,','connected at the wrist.');
text('BINDU',M,163,48,bold,navy);
para('Standalone Safety and Accessibility Wearable',M,226,CW,18,24,teal,bold);
rect(M,267,CW,47,color('FFF4E4'));label('Development stage',M+13,277);text('Pre-hardware integration - firmware scaffold and two parts ordered',M+13,294,9.5,regular,amber);
heading('The problem',337);
para('Older adults living independently may struggle to request help after a fall or emergency. Blind and low-vision users need accessible obstacle feedback, while caregivers need to know when a safety wearable has been removed.',M,368,CW,11,16);
label('Target users',M,432);
para('Older adults living independently, blind and low-vision adults, and their family caregivers. Each user can enable the functions relevant to their needs.',M,451,CW,10.5,15);
heading('Abstract',511);
para('Bindu is a standalone 4G wrist-wearable project combining physical SOS, motion-based fall detection, watch-removal alerts and gesture-activated obstacle awareness. A forward-facing distance sensor will drive vibration pulses that become faster as obstacles approach, with optional audible feedback.',M,542,CW,10.5,15);
para('An ESP32-S3 will coordinate local sensing and send remote events over Wi-Fi or LTE data to a self-hosted backend. The backend will request caregiver SMS through httpSMS and a dedicated Android gateway phone. The wearer will not need a nearby phone; remote delivery still depends on internet and gateway availability.',M,617,CW,10.5,15);
para('Development proceeds from subsystem tests to an integrated PCB and custom enclosure. Evaluation will measure gesture reliability, false alarms, ranging limits, alert delivery and power use. Location, vitals and Telugu/Hindi voice interaction are planned extensions. No detection or battery-performance results are claimed at this stage.',M,692,CW,10.5,15);

// Page 2: product interaction and truthful feature status.
base(2,'Product and user experience','One device. Clear interactions.','Four planned functions connect accessible local feedback with caregiver awareness.');
let y=175;
y=bullet('01  Physical SOS', 'A deliberate button press creates an emergency event. The watch uploads it over an available data connection; the backend records the event and requests caregiver SMS. A cancel input and clear local feedback are part of the design.',y);
y=bullet('02  Gesture-activated obstacle mode', 'Raise the forearm so the sensor faces forward, then perform the deliberate wrist gesture. Distinct vibration confirms activation. Faster pulses indicate a nearer obstacle; optional sound adds a close-obstacle warning. Repeat a validated gesture to deactivate.',y);
y=bullet('03  Watch-removal awareness', 'Confirm sustained contact loss before alerting the caregiver. Filter loose-strap events, provide an approved charging/removal interval, and report stable worn-again contact. Missing heart-rate readings alone are not proof of removal.',y);
y=bullet('04  Motion-based fall detection', 'Replace fixed sensor stubs with real IMU readings, then evaluate impact and stillness logic on controlled recordings. Tune false alarms and detection thresholds before enabling automatic caregiver alerts.',y);
card('Key engineering distinction','A watch-mounted IMU measures watch motion. Bending only the hand may leave the forearm and watch stationary. The requested gesture needs experiments and may require an additional wrist-bend sensor.',M,575,CW,94);
label('Wearable design direction',M,692);
para('Use the compact XIAO controller, an integrated carrier PCB, selected LTE module, protected flat battery and a custom wrist enclosure. Sensor direction, antenna clearance, optical contact and battery current capability must be checked together. Case dimensions and runtime are not yet validated.',M,712,CW,10,14);

// Page 3: architecture and final stack.
base(3,'System architecture','Local awareness. Remote alerts.','Selected architecture - software implementation and Kubernetes deployment are pending.');
const bw=150,gap=28;
function node(title,body,x,t,w=150,h=72){rect(x,t,w,h,light,line);text(title,x+11,t+10,10.3,bold,teal);para(body,x+11,t+29,w-22,9.2,12.5);}
function arrow(x1,t1,x2,t2){page.drawLine({start:{x:x1,y:H-t1},end:{x:x2,y:H-t2},thickness:1.3,color:teal});const a=Math.atan2(t2-t1,x2-x1);for(const d of [-0.5,0.5])page.drawLine({start:{x:x2,y:H-t2},end:{x:x2-6*Math.cos(a+d),y:H-(t2-6*Math.sin(a+d))},thickness:1.3,color:teal});}
node('BINDU WATCH','Sensing + local feedback\nWi-Fi / LTE data',M,177);node('BACKEND API','Express / TypeScript\nDevice authentication',M+bw+gap,177);node('CAREGIVER WEB','React / TypeScript\nLive SSE updates',M+2*(bw+gap),177,151);
arrow(194,213,222,213);arrow(372,213,400,213);
node('DATA + SMS WORKER','PostgreSQL event store\nOutbox + delivery retries',222,288);arrow(297,249,297,288);
node('httpSMS API','Server-side credentials\nSMS request + status',M,402);node('ANDROID GATEWAY','Team-managed phone\nInternet + active SIM',222,402);node('CAREGIVER SMS','SMS received on phone\nAcknowledgement separate',400,402,151);
arrow(222,324,119,324);arrow(119,324,119,402);arrow(194,438,222,438);arrow(372,438,400,438);
text('The gateway phone can stay with the team; it is not paired to the wearer.',M,489,9.4,regular,muted);
row(['Layer','Selected technology'],528,[117,CW-117],30,true);
const rows=[['Firmware','C, ESP-IDF, FreeRTOS; I2C, UART, cellular AT commands'],['Frontend','React, TypeScript, Vite; responsive caregiver dashboard'],['Backend / data','Node.js, TypeScript, Express, PostgreSQL; HTTPS + SSE'],['Deployment','Container images; existing Kubernetes VM; persistent storage'],['Notifications','httpSMS REST API and dedicated Android gateway phone']];
rows.forEach((r,i)=>row(r,558+i*38,[117,CW-117],38));
para('Open-source application stack; no new managed hosting subscription is required. Existing VM/storage, mobile data, carrier SMS and fabrication costs remain separate.',M,755,CW,9.2,12);

// Page 4: evidence, delivery, safeguards, proof of work.
base(4,'Readiness and delivery','What exists. What comes next.','A measurable delivery plan with explicit prototype status.');
card('Available in the repository','ESP-IDF/FreeRTOS task scaffold, event dispatch, priorities, power-mode state and component boundaries. Hardware inputs, SMS, voice and sync remain stubs; selected-board build is unverified.',M,171,CW,89);
card('Hardware ordered','XIAO ESP32-S3 Plus from HUBTRONICS; MPU6050 from Quartz. Receipt is not yet confirmed. Other sensors, cellular integration and the custom PCB/enclosure are pending.',M,272,CW,78);
heading('Delivery gates and acceptance evidence',373);
row(['Gate','Required evidence'],405,[128,CW-128],29,true);
const gates=[['1. SOS to caregiver','Wi-Fi disabled; LTE upload reaches backend and gateway SMS arrives. Measure latency and record failure/recovery.'],['2. Local assistance','Measure obstacle range limits, gesture false triggers and warning recognition in supervised tests.'],['3. Removal + motion','Test loose straps, charging, restored contact and controlled motion; record false alarms and missed events.'],['4. Wrist integration','Verify PCB fit, supply stability, antenna performance and measured battery runtime before final case assembly.']];
gates.forEach((r,i)=>row(r,434+i*49,[128,CW-128],49));
label('Operating boundaries',M,650);
para('Obstacle warnings supplement a cane or guide dog; a quiet sensor does not prove a clear path or detect every drop-off. Remote alerts require watch data, backend and Android gateway availability. httpSMS lists a free quota of up to 200 sent/received SMS per month; carrier allowances still apply. Persist alert jobs, deduplicate retries and distinguish API acceptance, delivery and caregiver acknowledgement.',M,668,CW,9.5,13);
label('Proof of work and technical references',M,740);
const links=[['Project repository','https://github.com/Rayudu0729/HealthBuddy'],['httpSMS API','https://docs.httpsms.com/'],['Plans','https://auth.httpsms.com/'],['Kubernetes','https://kubernetes.io/docs/concepts/workloads/']];
let lx=M;
for(const [name,url]of links){text(name,lx,759,8.5,regular,teal);const ww=regular.widthOfTextAtSize(name,8.5);page.drawLine({start:{x:lx,y:H-770},end:{x:lx+ww,y:H-770},color:teal,thickness:0.4});page.drawText('',{x:lx,y:H-759});page.doc; page.link(url,[lx,H-773,lx+ww,H-757]);lx+=ww+22;}
await fs.mkdir(root+'/output/pdf',{recursive:true});
await fs.writeFile(out,await pdf.save());

export {out};
