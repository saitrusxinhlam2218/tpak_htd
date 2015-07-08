#define KEY_SRC
#include "taxipak.h"
#include "Object.h"
#include "KelaNode_db.h"

void
fill_kelanode_key(req_buf, buf)
     char *req_buf;
     char *buf;
{
  KELANODE_REC *kn;
  register int offset = 0;

  kn = (KELANODE_REC *)req_buf;
  stchar(kn->rte_id, &buf[offset], sizeof(kn->rte_id)-1);
  offset += sizeof(kn->rte_id)-1;

  stint(kn->nbr, &buf[offset]);
  offset += INTSIZE;

  stlong(kn->tpak_id, &buf[offset]);
  offset += LONGSIZE;
  
}

void fill_kelanode(req_buf, buf, action)
     char *req_buf;
     char *buf;
     int action;
{
  KELANODE_REC *kn;
  register int offset = 0;

  kn = (KELANODE_REC *)req_buf;
  stchar(kn->rte_id, &buf[offset], sizeof(kn->rte_id)-1);
  offset += sizeof(kn->rte_id)-1;

  stint(kn->nbr, &buf[offset]);
  offset += INTSIZE;

  stlong(kn->tpak_id, &buf[offset]);
  offset += LONGSIZE;

  stchar(&kn->type, &buf[offset], sizeof(kn->type));
  offset += sizeof(kn->type);

  stchar(kn->street, &buf[offset], sizeof(kn->street)-1);
  offset += sizeof(kn->street)-1;

  stlong(kn->streetnbr, &buf[offset]);
  offset += LONGSIZE;

  stchar(kn->streetletter, &buf[offset], sizeof(kn->streetletter)-1);
  offset += sizeof(kn->streetletter)-1;

  stchar(kn->city, &buf[offset], sizeof(kn->city)-1);
  offset += sizeof(kn->city)-1;

  stchar(kn->pickuptime, &buf[offset], sizeof(kn->pickuptime)-1);
  offset += sizeof(kn->pickuptime)-1;

  stchar(kn->passenger, &buf[offset], sizeof(kn->passenger)-1);
  offset += sizeof(kn->passenger)-1;

  stchar(kn->phone, &buf[offset], sizeof(kn->phone)-1);
  offset += sizeof(kn->phone)-1;

  stdbl(kn->gps_lat, &buf[offset]);
  offset += DOUBLESIZE;

  stdbl(kn->gps_lon, &buf[offset]);
  offset += DOUBLESIZE;

  stchar(kn->arrival_time, &buf[offset], sizeof(kn->arrival_time)-1);
  offset += sizeof(kn->arrival_time)-1;

  stchar(kn->departure_time, &buf[offset], sizeof(kn->departure_time)-1);
  offset += sizeof(kn->departure_time)-1;

  stchar(kn->eta, &buf[offset], sizeof(kn->eta)-1);
  offset += sizeof(kn->eta)-1;

  stchar(kn->booking_id, &buf[offset], sizeof(kn->booking_id)-1);
  offset += sizeof(kn->booking_id)-1;

  stchar(kn->full_booking_id, &buf[offset], sizeof(kn->full_booking_id)-1);
  offset += sizeof(kn->full_booking_id)-1;

  stchar(kn->manual_descript, &buf[offset], sizeof(kn->manual_descript)-1);
  offset += sizeof(kn->manual_descript)-1;    


  
}

void
unfill_kelanode(req_buf, buf)
     char *req_buf;
     char *buf;
{
  KELANODE_REC *kn;
  register int offset = 0;

  kn = (KELANODE_REC *)req_buf;

  ldchar(&buf[offset], sizeof(kn->rte_id)-1, kn->rte_id);
  offset += sizeof(kn->rte_id)-1;

  kn->nbr = ldint(&buf[offset]);
  offset += INTSIZE;

  kn->tpak_id = ldlong(&buf[offset]);
  offset += LONGSIZE;
  
  kn->type = buf[offset];
  offset += sizeof(kn->type);

  ldchar(&buf[offset], sizeof(kn->street)-1, kn->street);
  offset += sizeof(kn->street)-1;  
  
  kn->streetnbr = ldlong(&buf[offset]);
  offset += LONGSIZE;  

  ldchar(&buf[offset], sizeof(kn->streetletter)-1, kn->streetletter);
  offset += sizeof(kn->streetletter)-1;

  ldchar(&buf[offset], sizeof(kn->city)-1, kn->city);
  offset += sizeof(kn->city)-1;

  ldchar(&buf[offset], sizeof(kn->pickuptime)-1, kn->pickuptime);
  offset += sizeof(kn->pickuptime)-1;      
  
  ldchar(&buf[offset], sizeof(kn->passenger)-1, kn->passenger);
  offset += sizeof(kn->passenger)-1;

  ldchar(&buf[offset], sizeof(kn->phone)-1, kn->phone);
  offset += sizeof(kn->phone)-1;

  kn->gps_lat = lddbl(&buf[offset]);
  offset += DOUBLESIZE;

  kn->gps_lon = lddbl(&buf[offset]);
  offset += DOUBLESIZE;

  ldchar(&buf[offset], sizeof(kn->arrival_time)-1, kn->arrival_time);
  offset += sizeof(kn->arrival_time)-1;

  ldchar(&buf[offset], sizeof(kn->departure_time)-1, kn->departure_time);
  offset += sizeof(kn->departure_time)-1;

  ldchar(&buf[offset], sizeof(kn->eta)-1, kn->eta);
  offset += sizeof(kn->eta)-1;

  ldchar(&buf[offset], sizeof(kn->booking_id)-1, kn->booking_id);
  offset += sizeof(kn->booking_id)-1;

  ldchar(&buf[offset], sizeof(kn->full_booking_id)-1, kn->full_booking_id);
  offset += sizeof(kn->full_booking_id)-1;

  ldchar(&buf[offset], sizeof(kn->manual_descript)-1, kn->manual_descript);
  offset += sizeof(kn->manual_descript)-1;          

}
