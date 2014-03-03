/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2014 Magister Solutions
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Budiarto Herman <budiarto.herman@magister.fi>
 *
 */

#ifndef SATELLITE_STATS_HELPER_H
#define SATELLITE_STATS_HELPER_H

#include <ns3/ptr.h>
#include <ns3/simple-ref-count.h>
#include <ns3/object.h>
#include <ns3/type-id.h>
#include <ns3/object-factory.h>
#include <ns3/string.h>
#include <ns3/callback.h>
#include <ns3/log.h>
#include <ns3/node-container.h>
#include <ns3/data-collection-object.h>
#include <ns3/probe.h>
#include <map>


namespace ns3 {


// SATELLITE STATS HELPER /////////////////////////////////////////////////////

class SatHelper;

/**
 * \brief Abstract class
 */
class SatStatsHelper : public SimpleRefCount<SatStatsHelper>
{
public:
  /**
   * \enum IdentifierType_t
   * \brief
   */
  typedef enum
  {
    IDENTIFIER_GLOBAL = 0,
    IDENTIFIER_GW,
    IDENTIFIER_BEAM,
    IDENTIFIER_UT,
    IDENTIFIER_UT_USER
  } IdentifierType_t;

  /**
   * \param identifierType
   * \return
   */
  static std::string GetIdentiferTypeName (IdentifierType_t identifierType);

  /**
   * \enum OutputType_t
   * \brief
   */
  typedef enum
  {
    OUTPUT_NONE = 0,
    OUTPUT_SCALAR_FILE,
    OUTPUT_SCATTER_FILE,
    OUTPUT_HISTOGRAM_FILE,
    OUTPUT_PDF_FILE,        // probability distribution function
    OUTPUT_CDF_FILE,        // cumulative distribution function
    OUTPUT_SCALAR_PLOT,
    OUTPUT_SCATTER_PLOT,
    OUTPUT_HISTOGRAM_PLOT,
    OUTPUT_PDF_PLOT,        // probability distribution function
    OUTPUT_CDF_PLOT,        // cumulative distribution function
  } OutputType_t;

  /**
   * \param outputType
   * \return
   */
  static std::string GetOutputTypeName (OutputType_t outputType);

  /**
   * \brief
   * \param satHelper
   */
  SatStatsHelper (Ptr<const SatHelper> satHelper);

  /// Destructor.
  virtual ~SatStatsHelper ();

  /**
   * \brief Install probes, collectors, and aggregators.
   *
   * Behaviour should be implemented by child class in DoInstall().
   */
  void Install ();

  /**
   * \param name
   */
  void SetName (std::string name);

  /**
   * \return
   */
  std::string GetName () const;

  /**
   * \param identifierType
   * \warning Does not have any effect if invoked after Install().
   */
  void SetIdentifierType (IdentifierType_t identifierType);

  /**
   * \return
   */
  IdentifierType_t GetIdentifierType () const;

  /**
   * \param outputType
   * \warning Does not have any effect if invoked after Install().
   */
  void SetOutputType (OutputType_t outputType);

  /**
   * \return
   */
  OutputType_t GetOutputType () const;

  /**
   * \return
   */
  Ptr<const SatHelper> GetSatHelper () const;

  /**
   * \return
   */
  Ptr<DataCollectionObject> GetAggregator () const;

  /**
   * \brief
   * \param ut
   * \return
   */
  static uint32_t GetUtId (Ptr<Node> ut);

  /**
   * \brief
   * \param utUser
   * \return
   */
  static uint32_t GetUtUserId (Ptr<Node> utUser);

protected:
  /**
   * \brief
   */
  virtual void DoInstall () = 0;

  ///
  typedef std::map<uint32_t, Ptr<DataCollectionObject> > CollectorMap_t;

  /**
   * \brief Create the aggregator according to the output type.
   */
  virtual void CreateAggregator ();

  /**
   * \brief
   * \param collectorMap a map of collectors where the newly created collectors
   *                     will be inserted into.
   * \return number of collectors created.
   */
  virtual uint32_t CreateTerminalCollectors (CollectorMap_t &collectorMap) const;

  /**
   * \brief Create a collector for each identifier.
   * \param collectorTypeId the type of collector to be created.
   * \param collectorMap a map of collectors where the newly created collectors
   *                     will be inserted into.
   * \return number of collectors created.
   *
   * The currently active identifier type (as returned by GetIdentifierType()
   * and indicated by the `IdentifierType` attribute) determines the number of
   * collectors created by this method. The active identifier type can be
   * modified by invoking the SetIdentifierType() method or modifying the
   * `IdentifierType` attribute.
   */
  virtual uint32_t CreateCollectors (std::string collectorTypeId,
                                     CollectorMap_t &collectorMap) const;

  /**
   * \brief Create a new probe and connect it to a specified object and
   *        collector.
   * \param object
   * \param objectTraceSourceName
   * \param probeName
   * \param probeTypeId
   * \param probeTraceSourceName
   * \param identifier
   * \param collectorMap
   * \param collectorTraceSink
   * \return pointer to the newly created probe, connected to the specified
   *         object and the right connector, or zero if failure happens.
   *
   * Assuming collectors are already created in the collectorMap.
   */
  template<typename R, typename C, typename P1, typename P2>
  Ptr<Probe> InstallProbe (Ptr<Object> object,
                           std::string objectTraceSourceName,
                           std::string probeName,
                           std::string probeTypeId,
                           std::string probeTraceSourceName,
                           uint32_t identifier,
                           CollectorMap_t &collectorMap,
                           R (C::*collectorTraceSink) (P1, P2)) const;

  /**
   * \brief
   * \param sourceCollectorMap
   * \param traceSourceName
   * \param targetCollectorMap
   * \param traceSink
   * \return
   */
  template<typename R, typename C, typename P1, typename P2>
  bool ConnectCollectorToCollector (CollectorMap_t &sourceCollectorMap,
                                    std::string traceSourceName,
                                    CollectorMap_t &targetCollectorMap,
                                    R (C::*traceSink) (P1, P2)) const;

  /**
   * \brief
   * \param sourceCollectorMap
   * \param traceSourceName
   * \param targetCollector
   * \param traceSink
   * \return
   */
  template<typename R, typename C, typename P1, typename P2>
  bool ConnectCollectorToCollector (CollectorMap_t &sourceCollectorMap,
                                    std::string traceSourceName,
                                    Ptr<DataCollectionObject> targetCollector,
                                    R (C::*traceSink) (P1, P2)) const;

  /**
   * \brief
   * \param sourceCollector
   * \param traceSourceName
   * \param targetCollectorMap
   * \param traceSink
   * \return
   */
  template<typename R, typename C, typename P1, typename P2>
  bool ConnectCollectorToCollector (Ptr<DataCollectionObject> sourceCollector,
                                    std::string traceSourceName,
                                    CollectorMap_t &targetCollectorMap,
                                    R (C::*traceSink) (P1, P2)) const;

  /**
   * \brief
   * \param sourceCollector
   * \param traceSourceName
   * \param targetCollector
   * \param traceSink
   * \return
   */
  template<typename R, typename C, typename P1, typename P2>
  bool ConnectCollectorToCollector (Ptr<DataCollectionObject> sourceCollector,
                                    std::string traceSourceName,
                                    Ptr<DataCollectionObject> targetCollector,
                                    R (C::*traceSink) (P1, P2)) const;

  /*
  template<typename R, typename C, typename P1, typename V1>
  void ConnectCollectorToAggregator (CollectorMap_t &collectorMap,
                                     std::string collectorTraceSourceName,
                                     R (C::*aggregatorTraceSink) (P1, V1));
  template<typename R, typename C, typename P1, typename V1, typename V2>
  void ConnectCollectorToAggregator (CollectorMap_t &collectorMap,
                                     std::string collectorTraceSourceName,
                                     R (C::*aggregatorTraceSink) (P1, V1, V2));
  template<typename R, typename C, typename P1, typename V1, typename V2, typename V3>
  void ConnectCollectorToAggregator (CollectorMap_t &collectorMap,
                                     std::string collectorTraceSourceName,
                                     R (C::*aggregatorTraceSink) (P1, V1, V2, V3));
  template<typename R, typename C, typename P1, typename V1, typename V2, typename V3, typename V4>
  void ConnectCollectorToAggregator (CollectorMap_t &collectorMap,
                                     std::string collectorTraceSourceName,
                                     R (C::*aggregatorTraceSink) (P1, V1, V2, V3, V4));
  template<typename R, typename C, typename P1, typename V1, typename V2, typename V3, typename V4, typename V5>
  void ConnectCollectorToAggregator (CollectorMap_t &collectorMap,
                                     std::string collectorTraceSourceName,
                                     R (C::*aggregatorTraceSink) (P1, V1, V2, V3, V4, V5));
  template<typename R, typename C, typename P1, typename V1, typename V2, typename V3, typename V4, typename V5, typename V6>
  void ConnectCollectorToAggregator (CollectorMap_t &collectorMap,
                                     std::string collectorTraceSourceName,
                                     R (C::*aggregatorTraceSink) (P1, V1, V2, V3, V4, V5, V6));
  */

  /**
   * \param utUserNode
   * \return
   */
  virtual uint32_t GetUtUserIdentifier (Ptr<Node> utUserNode) const;

  /**
   * \param utNode
   * \return
   */
  virtual uint32_t GetUtIdentifier (Ptr<Node> utNode) const;

  /**
   * \param beamId
   * \return
   */
  virtual uint32_t GetBeamIdentifier (uint32_t beamId) const;

  /**
   * \param gwNode
   * \return
   */
  virtual uint32_t GetGwIdentifier (Ptr<Node> gwNode) const;

private:
  std::string                m_name;            ///<
  IdentifierType_t           m_identifierType;  ///<
  OutputType_t               m_outputType;      ///<
  bool                       m_isInstalled;     ///<
  Ptr<const SatHelper>       m_satHelper;       ///<
  Ptr<DataCollectionObject>  m_aggregator;      ///<

  // TODO: Add support for more than one aggregators.

}; // end of class SatStatsHelper


// TEMPLATE METHOD DEFINITIONS ////////////////////////////////////////////////

template<typename R, typename C, typename P1, typename P2>
Ptr<Probe>
SatStatsHelper::InstallProbe (Ptr<Object> object,
                              std::string objectTraceSourceName,
                              std::string probeName,
                              std::string probeTypeId,
                              std::string probeTraceSourceName,
                              uint32_t    identifier,
                              SatStatsHelper::CollectorMap_t &collectorMap,
                              R (C::*collectorTraceSink) (P1, P2)) const
{
  // Confirm that the probe type contains the trace source.
  TypeId probeTid = TypeId::LookupByName (probeTypeId);
  NS_ASSERT (probeTid.LookupTraceSourceByName (probeTraceSourceName) != 0);

  // Create the probe.
  ObjectFactory factory;
  factory.SetTypeId (probeTid);
  factory.Set ("Name", StringValue (probeName));
  Ptr<Probe> probe = factory.Create ()->GetObject<Probe> (probeTid);

  // Connect the object to the probe.
  if (probe->ConnectByObject (objectTraceSourceName, object))
    {
      // Connect the probe to the right collector.
      SatStatsHelper::CollectorMap_t::iterator it = collectorMap.find (identifier);
      NS_ASSERT_MSG (it != collectorMap.end (),
                     "Unable to find collector with identifier " << identifier);
      Ptr<C> collector = it->second->GetObject<C> ();
      NS_ASSERT (collector != 0);

      if (probe->TraceConnectWithoutContext (probeTraceSourceName,
                                             MakeCallback (collectorTraceSink,
                                                           collector)))
        {
          return probe;
        }
      else
        {
          return 0;
        }
    }
  else
    {
      return 0;
    }

} // end of `InstallProbe`


template<typename R, typename C, typename P1, typename P2>
bool
SatStatsHelper::ConnectCollectorToCollector (SatStatsHelper::CollectorMap_t &sourceCollectorMap,
                                             std::string traceSourceName,
                                             SatStatsHelper::CollectorMap_t &targetCollectorMap,
                                             R (C::*traceSink) (P1, P2)) const
{
  NS_ASSERT (sourceCollectorMap.size () == targetCollectorMap.size ());

  for (SatStatsHelper::CollectorMap_t::iterator it1 = sourceCollectorMap.begin ();
       it1 != sourceCollectorMap.end (); ++it1)
    {
      const uint32_t identifier = it1->first;
      SatStatsHelper::CollectorMap_t::iterator it2 = targetCollectorMap.find (identifier);
      NS_ASSERT_MSG (it2 != targetCollectorMap.end (),
                     "Unable to find target collector with identifier " << identifier);

      if (!ConnectCollectorToCollector (it1->second, traceSourceName,
                                        it2->second, traceSink))
        {
          return false;
        }
    }

  return true;
}


template<typename R, typename C, typename P1, typename P2>
bool
SatStatsHelper::ConnectCollectorToCollector (SatStatsHelper::CollectorMap_t &sourceCollectorMap,
                                             std::string traceSourceName,
                                             Ptr<DataCollectionObject> targetCollector,
                                             R (C::*traceSink) (P1, P2)) const
{
  for (SatStatsHelper::CollectorMap_t::iterator it = sourceCollectorMap.begin ();
       it != sourceCollectorMap.end (); ++it)
    {
      if (!ConnectCollectorToCollector (it->second, traceSourceName,
                                        targetCollector, traceSink))
        {
          return false;
        }
    }

  return true;
}


template<typename R, typename C, typename P1, typename P2>
bool
SatStatsHelper::ConnectCollectorToCollector (Ptr<DataCollectionObject> sourceCollector,
                                             std::string traceSourceName,
                                             SatStatsHelper::CollectorMap_t &targetCollectorMap,
                                             R (C::*traceSink) (P1, P2)) const
{
  for (SatStatsHelper::CollectorMap_t::iterator it = targetCollectorMap.begin ();
       it != targetCollectorMap.end (); ++it)
    {
      if (!ConnectCollectorToCollector (sourceCollector, traceSourceName,
                                        it->second, traceSink))
        {
          return false;
        }
    }

  return true;
}


template<typename R, typename C, typename P1, typename P2>
bool
SatStatsHelper::ConnectCollectorToCollector (Ptr<DataCollectionObject> sourceCollector,
                                             std::string traceSourceName,
                                             Ptr<DataCollectionObject> targetCollector,
                                             R (C::*traceSink) (P1, P2)) const
{
  Ptr<C> target = targetCollector->GetObject<C> ();
  return sourceCollector->TraceConnectWithoutContext (traceSourceName,
                                                      MakeCallback (traceSink,
                                                                    target));
}


} // end of namespace ns3


#endif /* SATELLITE_STATS_HELPER_H */