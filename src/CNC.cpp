#include <CNC.h>

using namespace std;

point_t MachineCordinates;
point_t OffsetCordinates;
point_t OffsetValue;

gcode_t GcodePointer;

#define GCODE_TABLE_SIZE 4
gcode_t gcode_table[GCODE_TABLE_SIZE] = { //Supported G-Codes.
  //       G  X  Y  Z  F  R  Modal
  gcode_t{ 0, 0, 0, 0, 0, 0, true }, //Rapid
  gcode_t{ 1, 0, 0, 0, 0, 0, true }, //Line
  gcode_t{ 2, 0, 0, 0, 0, 0, true }, //Arc Clockwise
  gcode_t{ 3, 0, 0, 0, 0, 0, true }, //Arc Counter-Clockwise

  //Modal Gcodes are handled everytime we see a X||Y||Z change!
};

bool Hold = true;
bool Stop = true;
Stepper *Xaxis;
Stepper *Yaxis;

ifstream nc_file;
long nc_line = 0;
string nc_buffer;

void CNC_XPlus()
{
  MachineCordinates.x += ONE_STEP_DISTANCE;
  OffsetCordinates.x = MachineCordinates.x - OffsetValue.x;
  OffsetCordinates.y = MachineCordinates.y - OffsetValue.y;
}
void CNC_XMinus()
{
  MachineCordinates.x -= ONE_STEP_DISTANCE;
  OffsetCordinates.x = MachineCordinates.x - OffsetValue.x;
  OffsetCordinates.y = MachineCordinates.y - OffsetValue.y;
}
void CNC_YPlus()
{
  MachineCordinates.y += ONE_STEP_DISTANCE;
  OffsetCordinates.x = MachineCordinates.x - OffsetValue.x;
  OffsetCordinates.y = MachineCordinates.y - OffsetValue.y;
}
void CNC_YMinus()
{
  MachineCordinates.y -= ONE_STEP_DISTANCE;
  OffsetCordinates.x = MachineCordinates.x - OffsetValue.x;
  OffsetCordinates.y = MachineCordinates.y - OffsetValue.y;
}

void CNC_JogXPlus()
{
  if (Stop == false) return;
  Xaxis->SetFeedRate(RAPID_FEED);
  Xaxis->Step(+1);
  CNC_XPlus();
}
void CNC_JogXMinus()
{
  if (Stop == false) return;
  Xaxis->SetFeedRate(RAPID_FEED);
  Xaxis->Step(-1);
  CNC_XMinus();
}
void CNC_JogYPlus()
{
  if (Stop == false) return;
  Yaxis->SetFeedRate(RAPID_FEED);
  Yaxis->Step(+1);
  CNC_YPlus();
}
void CNC_JogYMinus()
{
  if (Stop == false) return;
  Yaxis->SetFeedRate(RAPID_FEED);
  Yaxis->Step(-1);
  CNC_YMinus();
}
void CNC_Hold()
{
  Hold = true;
  printf("Hold!\n");
}
void CNC_Stop()
{
  Stop = true;
  printf("Stop!\n");
  nc_file.close();
}
void CNC_Start()
{
  if (!nc_file.is_open())
  {
    Stop = false;
    nc_file.open("test.nc");
  }
  Hold = false;
  printf("Start!\n");
}
void CNC_SetOrigin()
{
  printf("Setting Origin!\n");
  if (Stop == true) //We can't set origin while program is running!
  {
    OffsetValue.x = MachineCordinates.x;
    OffsetValue.y = MachineCordinates.y;

    OffsetCordinates.x = MachineCordinates.x - OffsetValue.x;
    OffsetCordinates.y = MachineCordinates.y - OffsetValue.y;
  }
}
bool InTolerance(float a, float b, float t)
{
  float diff;
  if (a > b)
  {
    diff = a - b;
  }
  else
  {
    diff = b - a;
  }
  //printf("(geoInTolerance) Difference: %.6f, Plus: %.6f, Minus: %.6f\n", diff, fabs(t), -fabs(t));
  if (diff <= fabs(t) && diff >= -fabs(t))
  {
    return true;
  }
  else
  {
    return false;
  }
}

float feed = 0;

float fxy, dx, dy = 0;
float x2, y2 = 0;
float xo, yo = 0;
//true = 1
int int_rad, int_radrad, int_xo, int_yo = 0;
int int_fxy, int_dx, int_dy, int_y2, int_x2 = 0;
int binrep = 0;

bool d, f, a, b = 0;
void CNC_Tick()
{
  if (nc_file.is_open())
  {
    if (GcodePointer.MoveDone == false)
    {
      //printf("Move Done = false\n");
      if (Hold == false)
      {
        if (GcodePointer.G == 0 || GcodePointer.G == 1)
        {
          if (GcodePointer.G == 0)
          {
            feed = RAPID_FEED;
          }
          else
          {
            feed = GcodePointer.F;
          }
          if (GcodePointer.FirstInstruction == true)
          {
            //Calculate meta_data
            fxy = dx = dy = y2 = x2 = 0;

            GcodePointer.line_meta.start_pos = OffsetCordinates;
            GcodePointer.FirstInstruction = false;

            dy = GcodePointer.Y - GcodePointer.line_meta.start_pos.y;
            if (dy < 0)
            {
              yo = -1;
            }
            else
            {
              yo = 1;
            }
            dy = fabs(dy);

            dx = GcodePointer.X - GcodePointer.line_meta.start_pos.x;
            if (dx < 0)
            {
              xo = -1;
            }
            else
            {
              xo = 1;
            }
            dx = fabs(dx);

            fxy = dx - dy;

            //printf("\tDirection -> dx = %0.4f, dy = %0.4f, yo = %0.4f, xo = %0.4f, initial_fxy = %0.4f\n", dx, dy, yo, xo, fxy);
            printf("\tLine Move to X%0.4f, Y%0.4f\n", GcodePointer.X, GcodePointer.Y);

          }
          else
          {
            //printf("FXY: %0.4f\n", fxy);
            if (fxy > 0)
            {
              if (xo > 0)
              {
                Xaxis->SetFeedRate(feed);
                Xaxis->Step(+1);
                CNC_XPlus();
              }
              else
              {
                Xaxis->SetFeedRate(feed);
                Xaxis->Step(-1);
                CNC_XMinus();
              }
              x2++;
              fxy = fxy - dy;
            }
            else
            {
              if (yo > 0)
              {
                Yaxis->SetFeedRate(feed);
                Yaxis->Step(+1);
                CNC_YPlus();
              }
              else
              {
                Yaxis->SetFeedRate(feed);
                Yaxis->Step(-1);
                CNC_YMinus();
              }
              y2++;
              fxy = fxy + dx;
            }
          }
          if (InTolerance(OffsetCordinates.x, GcodePointer.X, (ONE_STEP_DISTANCE + 0.0001)) && InTolerance(OffsetCordinates.y, GcodePointer.Y, (ONE_STEP_DISTANCE + 0.0001)))
          {
            printf("\t\tReached line endpoint!\n");
            GcodePointer.MoveDone = true;
          }
        }
        if (GcodePointer.G == 2 || GcodePointer.G == 3)
        {
          if (GcodePointer.FirstInstruction == true)
          {
            GcodePointer.arc_meta.start_pos = OffsetCordinates;
            GcodePointer.FirstInstruction = false;
            //Generate points on arc
            int_x2, int_y2, int_rad, int_radrad, int_xo, int_yo = 0;

            int_x2 = (int)OffsetCordinates.x/ ONE_STEP_DISTANCE;
            int_y2 = (int)OffsetCordinates.y / ONE_STEP_DISTANCE;

            int_rad = (int)GcodePointer.R / ONE_STEP_DISTANCE;

            int_radrad = int_rad * int_rad;

            if (GcodePointer.G == 2)
            {
              d = false; //Clockwise!
              printf("\tClockwise Arc until we reach X%0.4f, Y%0.4f at R%0.4f at F%0.4f\n", GcodePointer.X, GcodePointer.Y, GcodePointer.R, GcodePointer.F);
            }
            if (GcodePointer.G == 3)
            {
              d = true; //Counter-Clockwise!
              printf("\tCounter-Clockwise Arc until we reach X%0.4f, Y%0.4f at R%0.4f at F%0.4f\n", GcodePointer.X, GcodePointer.Y, GcodePointer.R, GcodePointer.F);
            }
            feed = GcodePointer.F;



          }
          else
          {
            int_fxy = (int_x2 * int_x2) + (int_y2 * int_y2) - int_radrad;
            int_dx = 2 * int_x2;
            int_dy = 2 * int_y2;
            //printf("fxy = %0.4f, dx = %0.4f, dy = %0.4f\n", fxy, dx, dy);

            f = false;
            a = false;
            b = false;

            if (int_fxy < 0) //Positive when outside circle, negative when inside circle
            {
              f = true;
            }
            if ( int_dx < 0)
            {
              a = true;
            }
            if ( int_dy < 0)
            {
              b = false;
            }

            binrep = 0;
            int_xo = 0;
            int_yo = 0;

            if (d)
            {
              binrep = binrep + 8;
            }
            if (f)
            {
              binrep = binrep + 4;
            }
            if (a)
            {
              binrep = binrep + 2;
            }
            if (b)
            {
              binrep = binrep + 1;
            }
            switch(binrep)
            {
              case 0:   int_yo = -1; break;
              case 1:   int_xo = -1; break;
              case 2:   int_xo = 1; break;
              case 3:   int_yo = 1; break;
              case 4:   int_xo = 1; break;
              case 5:   int_yo = 1; break;
              case 6:   int_yo = 1; break;
              case 7:   int_xo = -1; break;
              case 8:   int_xo = -1; break;
              case 9:   int_yo = 1; break;
              case 10:   int_yo = -1; break;
              case 11:   int_xo = 1; break;
              case 12:   int_yo = 1; break;
              case 13:   int_xo = 1; break;
              case 14:   int_xo = -1; break;
              case 15:   int_yo = -1; break;
            }

            if (int_yo < 0)
            {
              Yaxis->SetFeedRate(feed);
              Yaxis->Step(-1);
              CNC_YMinus();
            }
            if (int_yo > 0)
            {
              Yaxis->SetFeedRate(feed);
              Yaxis->Step(+1);
              CNC_YPlus();
            }
            if (int_xo < 0)
            {
              Xaxis->SetFeedRate(feed);
              Xaxis->Step(-1);
              CNC_XMinus();
            }
            if (int_xo > 0)
            {
              Xaxis->SetFeedRate(feed);
              Xaxis->Step(+1);
              CNC_XPlus();
            }

            int_x2 = int_x2 + int_xo;
            int_y2 = int_y2 + int_yo;
            //printf("Arc Tick Tock -> binrep = %d, xo = %0.4f, yo = %0.4f\n", binrep, xo, yo);

            if (InTolerance(OffsetCordinates.x, GcodePointer.X, (ONE_STEP_DISTANCE + 0.0005)) && InTolerance(OffsetCordinates.y, GcodePointer.Y, (ONE_STEP_DISTANCE + 0.0005)))
            {
              printf("\t\tReached arc endpoint!\n");
              GcodePointer.MoveDone = true;
            }
            //printf("X = %0.4f, Y = %0.4f\n", x2, y2);
          }
        }

        //Drive axis to position and set GcodePointer.MoveDone = true;
      }
    }
    else
    {
      //printf("Move Done = true\n");
      //Move is done, read nc file for next instruction, then set GcodePointer.MoveDone = false;

      if (getline (nc_file, nc_buffer) == 0)
      {
        Stop = true;
        printf("End of file!\n");
        nc_file.close(); //End of file!
      }
      else
      {

        transform(nc_buffer.begin(), nc_buffer.end(), nc_buffer.begin(), ::toupper);
        printf("NC Line> %s\n", nc_buffer.c_str());

        if (nc_buffer[0] == '(') return; //Ignore comments
        if (nc_buffer[0] == '/') return; //Ignore comments
        if (nc_buffer[0] == '#') return; //Ignore comments

        bool valid = false;
        string number = "";
        float val = 0;
        string letter = "";
        string::size_type sz;
        for (int i = 0; i < nc_buffer.size(); i++)
        {
          if (nc_buffer[i] == 'G' || nc_buffer[i] == 'X' || nc_buffer[i] == 'Y' || nc_buffer[i] == 'Z' || nc_buffer[i] == 'F' || nc_buffer[i] == 'R')
          {
            letter.push_back(nc_buffer[i]);
            while(i < nc_buffer.size())
            {
              if (nc_buffer[i+1] != '.' && isalpha(nc_buffer[i+1]))
              {
                break;
              }
              number.push_back(nc_buffer[i+1]);
              i++;
            }
            val = stof(number, &sz);

            //printf("Letter: %s, Value: %0.4f\n", letter.c_str(), val);
            if (letter == "G")
            {
              for (int z = 0; z < GCODE_TABLE_SIZE; z++)
              {
                if (val == gcode_table[z].G)
                {
                  if (gcode_table[z].Modal == true)
                  {
                    valid = true;
                    GcodePointer.G = val;
                  }
                  else
                  {
                    //Handle non modal code, make sure they don't go into pointer!
                  }
                  break;
                }
              }

            }
            if (letter == "X")
            {
              valid = true;
              GcodePointer.X = val;
            }
            if (letter == "Y")
            {
              valid = true;
              GcodePointer.Y = val;
            }
            if (letter == "Z")
            {
              valid = true;
              GcodePointer.Z = val;
            }
            if (letter == "F")
            {
              GcodePointer.F = val;
            }
            if (letter == "R")
            {
              GcodePointer.R = val;
            }

            number = "";
            letter = "";
          }
        }
        if (valid == true)
        {
          GcodePointer.MoveDone = false;
          GcodePointer.FirstInstruction = true;
        }
        else
        {
          return;
        }

        nc_line++;
      }
    }
  }
  else
  {
    //printf("File is not open!\n");
    GcodePointer.MoveDone = true;
    delay(500);
  }
}
void *cnc_thread(void *p)
{
  while(!quit)
  {
    CNC_Tick();
  }
}
void CNC_Init()
{
  wiringPiSetup();
  Xaxis = new Stepper(200, 29, 28, 27, 26);
  Yaxis = new Stepper(200, 25, 24, 23, 22);

  //Home maching to establish Machine Cordinates

  MachineCordinates.x = 0;
  MachineCordinates.y = 0;

  OffsetCordinates.x = 0;
  OffsetCordinates.y = 0;

  OffsetValue.x = 0;
  OffsetValue.y = 0;

  GcodePointer.MoveDone = true;

  pthread_t cnc_thread_handle;

  /* create a second thread which executes inc_x(&x) */
  if(pthread_create(&cnc_thread_handle, NULL, cnc_thread, NULL))
  {
    fprintf(stderr, "Error creating CNC thread\n");
  }

}
