/*
 *  diffusion_connection.h
 *
 *  This file is part of NEST.
 *
 *  Copyright (C) 2004 The NEST Initiative
 *
 *  NEST is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NEST is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NEST.  If not, see <http://www.gnu.org/licenses/>.
 *
 */


/* BeginDocumentation
Name: diffusion_connection - Synapse type for instantaneous rate connections
between neurons of type siegert_neuron.


Description:
 diffusion_connection is a connector to create instantaneous connections
 between neurons of type siegert_neuron. The connection type is identical to
 type rate_connection for instantaneous rate connections except for the two
 parameters "drift_factor" and "diffusion_factor" substituting the
 parameter "weight".

 The values of the parameters "delay" and "weight" are ignored for
 connections of this type.

Transmits: DiffusionEvent

References:

 Hahne, J., Dahmen, D., Schuecker, J., Frommer, A.,
 Bolten, M., Helias, M. and Diesmann, M. (2017).
 Integration of Continuous-Time Dynamics in a
 Spiking Neural Network Simulator.
 Front. Neuroinform. 11:34. doi: 10.3389/fninf.2017.00034

Author: David Dahmen, Jan Hahne, Jannis Schuecker
SeeAlso: siegert_neuron, rate_connection,
*/


#ifndef DIFFUSION_CONNECTION_H
#define DIFFUSION_CONNECTION_H

#include "connection.h"

namespace nest
{

/**
 * Class representing a rate connection. A rate connection
 * has the properties weight, delay and receiver port.
 */

template < typename targetidentifierT >
class DiffusionConnection : public Connection< targetidentifierT >
{

  double weight_;
  double drift_factor_;
  double diffusion_factor_;

public:
  // this line determines which common properties to use
  typedef CommonSynapseProperties CommonPropertiesType;
  typedef Connection< targetidentifierT > ConnectionBase;
  typedef DiffusionEvent EventType;


  /**
   * Default Constructor.
   * Sets default values for all parameters. Needed by GenericConnectorModel.
   */
  DiffusionConnection()
    : ConnectionBase()
    , weight_( 1.0 )
    , drift_factor_( 1.0 )
    , diffusion_factor_( 1.0 )
  {
  }


  // Explicitly declare all methods inherited from the dependent base
  // ConnectionBase.
  // This avoids explicit name prefixes in all places these functions are used.
  // Since ConnectionBase depends on the template parameter, they are not
  // automatically
  // found in the base class.
  using ConnectionBase::get_delay_steps;
  using ConnectionBase::get_rport;
  using ConnectionBase::get_target;


  void
  check_connection( Node& s,
    Node& t,
    rport receptor_type,
    double,
    const CommonPropertiesType& )
  {
    EventType ge;

    s.sends_secondary_event( ge );
    ge.set_sender( s );
    Connection< targetidentifierT >::target_.set_rport(
      t.handles_test_event( ge, receptor_type ) );
    Connection< targetidentifierT >::target_.set_target( &t );
  }

  /**
   * Send an event to the receiver of this connection.
   * \param e The event to send
   * \param p The port under which this connection is stored in the Connector.
   * \param t_lastspike Time point of last spike emitted
   */
  void
  send( Event& e, thread t, double, const CommonSynapseProperties& )
  {
    e.set_weight( weight_ );
    e.set_drift_factor( drift_factor_ );
    e.set_diffusion_factor( diffusion_factor_ );
    e.set_receiver( *get_target( t ) );
    e.set_rport( get_rport() );
    e();
  }

  void get_status( DictionaryDatum& d ) const;

  void set_status( const DictionaryDatum& d, ConnectorModel& cm );

  void
  set_weight( double w )
  {
    throw BadProperty(
      "Please uses the parameters \"drift_factor\" and "
      "\"diffusion_factor\" to specifiy the weights" );
  }
  void
  set_delay( double )
  {
    throw BadProperty( "diffusion_connection has no delay." );
  }
};

template < typename targetidentifierT >
void
DiffusionConnection< targetidentifierT >::get_status( DictionaryDatum& d ) const
{
  ConnectionBase::get_status( d );
  def< double >( d, names::weight, weight_ );
  def< double >( d, names::drift_factor, drift_factor_ );
  def< double >( d, names::diffusion_factor, diffusion_factor_ );
  def< long >( d, names::size_of, sizeof( *this ) );
}

template < typename targetidentifierT >
void
DiffusionConnection< targetidentifierT >::set_status( const DictionaryDatum& d,
  ConnectorModel& cm )
{
  // If the delay is set, we throw a BadProperty
  if ( d->known( names::delay ) )
    throw BadProperty( "diffusion_connection has no delay." );
  // If the parameter weight is set, we throw a BadProperty
  if ( d->known( names::weight ) )
    throw BadProperty(
      "Please uses the parameters \"drift_factor\" and "
      "\"diffusion_factor\" to specifiy the weights" );

  ConnectionBase::set_status( d, cm );
  updateValue< double >( d, names::drift_factor, drift_factor_ );
  updateValue< double >( d, names::diffusion_factor, diffusion_factor_ );
}

} // namespace

#endif /* #ifndef DIFFUSION_CONNECTION_H */